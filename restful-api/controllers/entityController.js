const mongoose = require("mongoose");
const zlib = require("zlib");
const entityModel = require("../models/entity");
const asyncModelOperations = require("../assistants/asyncModelOperations");
const fsController = require("../controllers/fsController");
const socketController = require("../controllers/socketController");
const metadataAssistant = require("../assistants/metadataAssistant");
const tar = require("tar-stream");
const streams = require("memory-streams");
const sharp = require("sharp");
const md5 = require("md5");

const getMetadataFromBody = (checkGroup, checkRaw, req) => {
  if (req.body === null || !(req.body instanceof Object)) {
    throw "Invalid metadata for entity";
  }
  const metadata = req.body;
  const metadataMissingMessage = "Required attributes missing in metadata:";
  //Check that body contains special attributes used for define entity
  if (checkGroup && typeof metadata.group === "undefined") {
    throw metadataMissingMessage + " 'Group'";
  }
  if (checkRaw && typeof metadata.raw === "undefined") {
    throw metadataMissingMessage + " 'Raw'";
  }
  if (
    typeof metadata.tags === "undefined" ||
    !(metadata.tags instanceof Array) ||
    metadata.tags.length === 0
  ) {
    throw metadataMissingMessage + " 'tags'";
  }
  return metadata;
};

const createMetadataStartup = (filename, username, useremail) => {
  const tags = filename.split(/[\s,._-]+/);
  return (metadata = {
    creator: {
      name: username,
      email: useremail
    },
    name: filename,
    thumb: "",
    tags: tags,
    deps: []
  });
};

const createEntityFromMetadata = async (
  content,
  project,
  group,
  isPublic,
  isRestricted,
  cleanMetadata,
  sendBroadcast,
  presetThumb = null
) => {
  try {
    let filePath = getFilePath(project, group, cleanMetadata.name);
    //Check content exists in project and group
    const copyEntity = await checkFileExists(
      project,
      group,
      cleanMetadata.hash
    );
    if (copyEntity === null) {
      //Upload file to S3
      // console.log( "Adding: ", filePath );
      let savedFilename = { changed: false, name: filePath };
      await fsController.cloudStorageGetFilenameAndDuplicateIfExists(
        filePath,
        "eventhorizonentities",
        savedFilename
      );
      if (savedFilename["changed"] == true) {
        const entityToDelete = await getEntityByName(
          project,
          group,
          cleanMetadata.name
        );
        await deleteEntityComplete(project, entityToDelete);
        // const nn = savedFilename["name"];
        // cleanMetadata["name"] = nn.substring( nn.lastIndexOf("/")+1, nn.length);
      }

      // const gtr = groupThumbnailCalcRule(group);
      // const dep0Entity = await getEntityByHash(
      //   deps[0].metadata.hash,
      //   project
      // );
      // const content = await getEntityContent(
      //   dep0Entity._id,
      //   dep0Entity.project
      // );
      cleanMetadata.thumb = await thumbFromContent(
        content,
        presetThumb,
        groupThumbnailCalcRule(group)
      );

      // cleanMetadata.thumb = await thumbFromContent(
      //   content,
      //   groupThumbnailCalcRule(group)
      // );

      // Hashing of content
      metadataAssistant.udpateMetadata(cleanMetadata, content);

      // filePath = savedFilename["name"];
      await fsController.cloudStorageFileUpload(
        content,
        filePath,
        "eventhorizonentities"
      );

      //Create entity
      let entity = await createEntity(
        project,
        group,
        isPublic,
        isRestricted,
        cleanMetadata
      );

      if (entity !== null) {
        let json = {
          msg: "entityAdded",
          data: entity
        };
        if (sendBroadcast) {
          socketController.sendMessageToAllClients(JSON.stringify(json));
        }
      }

      return entity;
    }
  } catch (error) {
    console.log("Cannot create entity: ", error);
    return null;
  }
};

const createEntityFromMetadataToBeCleaned = async (project, metadata) => {
  const {
    content,
    group,
    isPublic,
    isRestricted,
    cleanMetadata
  } = cleanupMetadata(metadata);

  return await createEntityFromMetadata();
};

const createEntitiesFromContainer = async (project, containerBody) => {
  let container = tar.extract();
  const metadatas = [];
  const entities = [];

  container.on("entry", function(header, stream, next) {
    console.log(header.name);
    var writer = new streams.WritableStream();
    stream.pipe(writer);
    stream.on("end", function() {
      console.log(writer.toString());
      metadatas.push(writer.toString());
      next(); // ready for next entry
    });
    stream.resume(); // just auto drain the stream
  });

  const deflatedBody = zlib.inflateSync(new Buffer.from(containerBody));

  var reader = new streams.ReadableStream(deflatedBody);
  await reader.pipe(container);
  console.log("**************");
  reader.on("finish", () => {
    console.log("############### ");
    // all entries read
    // res.status(200).send(null);
  });

  metadatas.forEach(element => {
    console.log("############### ");
    // const newEntity = await createEntityFromMetadata( project, element );
    // if ( newEntity !== null ) entities.push(newEntity);
  });

  return entities;
  // console.log(deflatedBody);
};

const cleanupMetadata = metadata => {
  const result = {};

  if (typeof metadata.raw !== "undefined") {
    try {
      result.content = zlib.inflateSync(
        new Buffer.from(metadata.raw, "base64")
      );
    } catch (error) {
      result.content = new Buffer.from(metadata.raw, "base64");
    }
  } else {
    result.content = null;
  }
  result.group = metadata.group;
  result.keys = metadata.tags;
  result.isPublic = metadata.isPublic || false;
  result.isRestricted = metadata.isRestricted || false;
  //Remove service attributes
  delete metadata.raw;
  delete metadata.group;
  delete metadata.isPublic;
  delete metadata.isRestricted;
  //Add hash attribute
  result.cleanMetadata = metadata;

  return result;
};

const getFilePath = (project, group, name) => {
  return project + "/" + group + "/" + name;
};

const checkFileExists = async (project, group, hash) => {
  const query = {
    $and: [{ "metadata.hash": hash }, { group: group }, { project: project }]
  };
  const result = await entityModel.findOne(query);
  return result !== null ? result.toObject() : null;
};

const createEntity = async (
  project,
  group,
  isPublic,
  isRestricted,
  metadata
) => {
  const newEntityDB = new entityModel({
    project: project,
    group: group,
    isPublic: isPublic,
    isRestricted: isRestricted,
    metadata: metadata
  });
  await newEntityDB.save();
  return newEntityDB.toObject();
};

const updateEntity = async (
  entityId,
  project,
  group,
  isPublic,
  isRestricted,
  metadata
) => {
  const query = { _id: mongoose.Types.ObjectId(entityId) };

  await entityModel.updateOne(query, {
    project: project,
    group: group,
    isPublic: isPublic,
    isRestricted: isRestricted,
    metadata: metadata
  });
};

const deleteEntity = async entityId => {
  await entityModel.deleteOne({ _id: mongoose.Types.ObjectId(entityId) });
};

const deleteEntityComplete = async (project, entity) => {
  if (entity) {
    currentEntity = entity;
    console.log("[INFO] deleting entity " + currentEntity.metadata.name);
    const group = currentEntity.group;
    //Remove current file from S3
    await fsController.cloudStorageDelete(
      module.exports.getFilePath(project, group, currentEntity.metadata.name),
      "eventhorizonentities"
    );
    //Delete existing entity
    await module.exports.deleteEntity(currentEntity._id);
  }
};

const getEntityByIdProject = async (project, entityId, returnPublic) => {
  let query;
  if (returnPublic) {
    query = {
      $and: [
        { _id: mongoose.Types.ObjectId(entityId) },
        { $or: [{ project: project }, { isPublic: true }] }
      ]
    };
  } else {
    query = {
      $and: [{ _id: mongoose.Types.ObjectId(entityId) }, { project: project }]
    };
  }
  const result = await entityModel.findOne(query);

  return result !== null ? result.toObject() : null;
};

const updateById = async (entityId, updatedEntity) => {
  let query;
  query = { _id: entityId };
  const result = await entityModel.findOneAndUpdate(query, updatedEntity);

  return result !== null ? result.toObject() : null;
};

const getFileName = pathname => {
  return pathname
    .split("\\")
    .pop()
    .split("/")
    .pop();
};

const checkCommonFileExtension = (group, ext) => {
  if (group === "geom") {
    if (ext === "glb" || ext === "fbx") return true;
  } else if (group === "material") {
    if (ext === "zip") return true;
  } else if (group === "image") {
    if (
      ext === "jpeg" ||
      ext === "png" ||
      ext === "jpg" ||
      ext === "exr" ||
      ext === "tga" ||
      ext === "tiff" ||
      ext === "gif"
    ) {
      return true;
    }
  }

  return false;
};

const checkFileExtensionsOnEntityGroup = (group, filename) => {
  const ext = filename
    .split(".")
    .pop()
    .toLowerCase();

  return checkCommonFileExtension(group, ext);
};

const decompressZipppedEntityDeps = async (
  zip,
  project,
  username,
  useremail
) => {
  let fileList = [];
  let deps = [];
  zip.forEach((relativePath, zipEntry) => {
    if (zipEntry.dir === false) {
      fileList.push(zipEntry);
    }
  });

  for (let zipEntry of fileList) {
    const content = await zipEntry.async("uint8array");
    if (checkFileExtensionsOnEntityGroup("image", zipEntry.name)) {
      const depGruop = "image";

      const metadatadep = createMetadataStartup(
        getFileName(zipEntry.name),
        username,
        useremail
      );

      // Resize image to 512x512
      const fullSizeImage = Buffer.from(content);
      const scaledDown = await sharp(fullSizeImage)
        .resize(512, 512, { fit: "inside", withoutEnlargement: true })
        .toFormat("jpg")
        .toBuffer();
      const entity = await createEntityFromMetadata(
        scaledDown,
        project,
        depGruop,
        false,
        false,
        metadatadep,
        false,
        null
      );

      deps.push(entity);
    }
  }

  return deps;
};

const gtr_dep0_image = "dep0";
const gtr_content_image = "content";
const gtr_content_vector = "content_vector";
const gtr_content_default = "content_default";

const groupThumbnailCalcRule = group => {
  let contentType = null;
  if (group === "material") {
    contentType = gtr_dep0_image;
  } else if (group === "image") {
    contentType = gtr_content_image;
  } else if (group === "profile") {
    contentType = gtr_content_vector;
  } else if (group === "geom") {
    contentType = gtr_content_default;
  }
  return contentType;
};

const groupThumbnailSourceContent = async (entity, gtr) => {
  try {
    if (gtr === gtr_content_image || gtr === gtr_content_vector) {
      return await getEntityContent(entity._id, entity.project);
    } else if (gtr === gtr_dep0_image) {
      for (element of entity.metadata.deps) {
        if (element.key === "image") {
          const dep0Entity = await getEntityByHash(
            element.value[0],
            entity.project
          );
          return await getEntityContent(dep0Entity._id, dep0Entity.project);
        }
      }
    } else if (gtr === gtr_content_default) {
      return "";
    }
  } catch (error) {
    console.log("groupThumbnailSourceContent failed. Reason: " + error);
    return null;
  }
};

const thumbFromContent = async (content, presetThumb, gtr) => {
  let thumbBuff = null;
  if (gtr === gtr_content_vector) {
    thumbBuff = content;
  } else if (gtr === gtr_content_image) {
    thumbBuff = await sharp(content)
      .resize(64, 64, { fit: "inside", withoutEnlargement: true })
      .toFormat("jpg")
      .toBuffer();
  } else if (gtr === gtr_dep0_image) {
    thumbBuff = presetThumb;
  }
  return thumbBuff === null ? "" : thumbBuff.toString("base64");
};

const upsertThumb = async entityId => {
  const entity = await getEntityById(entityId);
  if (entity.metadata.thumb.length == 0) {
    try {
      const gtr = groupThumbnailCalcRule(entity.group);
      const content = await groupThumbnailSourceContent(entity, gtr);
      entity.metadata.thumb = await thumbFromContent(content.Body, null, gtr);
      return await updateById(entityId, entity);
    } catch (error) {
      console.log("Upsert thumb on id " + entityId + " failed. Cause:" + error);
      return null;
    }
  }
  return {};
};

const upsertTags = async (entityId, tags) => {
  try {
    const entity = await getEntityById(entityId);
    entity.metadata.tags = tags;
    await updateById(entityId, entity);
    return 201;
  } catch (error) {
    console.log("Upsert tags on id " + entityId + " failed. Cause:" + error);
    return 204;
  }
  return 204;
};

const getEntityContent = async (entityId, project) => {
  //Check existing entity for use project (or public)
  const currentEntity = await getEntityByIdProject(project, entityId, true);
  if (currentEntity === null) {
    throw "Invalid entity for user project";
  }
  const filePath = getFilePath(
    currentEntity.project,
    currentEntity.group,
    currentEntity.metadata.name
  );
  return await fsController.cloudStorageEntityGet(filePath);
};

const getEntityById = async entityId => {
  let query;
  query = { _id: entityId };
  const result = await entityModel.findOne(query);

  return result !== null ? result.toObject() : null;
};

const getEntityByHash = async (entityId, project) => {
  let query;
  query = { project: project, "metadata.hash": entityId };
  const result = await entityModel.findOne(query);

  return result !== null ? result.toObject() : null;
};

const getEntityByName = async (project, group, name) => {
  let query;
  query = { project: project, group: group, "metadata.name": name };
  const result = await entityModel.findOne(query);

  return result !== null ? result.toObject() : null;
};

const getEntitiesOfProject = async (project, returnPublic) => {
  let query;
  if (returnPublic) {
    query = [{ project: project }, { isPublic: true }];
  } else {
    query = { project: project };
  }
  const result = await entityModel.find(query);

  return result !== null ? result : null;
};

const getEntitiesOfProjectWithGroup = async (
  project,
  groupID,
  returnPublic
) => {
  let query;
  if (returnPublic) {
    query = { project: project, group: groupID, isPublic: true };
  } else {
    query = { project: project, group: groupID };
  }
  const result = await entityModel.find(query);

  return result !== null ? result : null;
};

const getEntitiesIdOfProjectWithGroup = async (project, groupID) => {
  const query = { project: project, group: groupID };
  const result = await entityModel.find(query, "id");

  return result !== null ? result : null;
};

const getEntitiesByProjectGroupTags = async (
  project,
  group,
  tags,
  fullData,
  randomElements
) => {
  const aggregationQueries = [
    {
      $match: {
        $and: [
          { isRestricted: false },
          { group: group },
          {
            $or: [{ project: project }, { isPublic: true }]
          },
          {
            $or: [
              {
                "metadata.tags": {
                  $all: tags
                }
              },
              { "metadata.name": tags[0] },
              {
                _id:
                  tags[0].length == 12 || tags[0].length == 24
                    ? mongoose.Types.ObjectId(tags[0])
                    : "DDDDDDDDDDDD"
              }
            ]
          }
        ]
      }
    }
  ];

  if (fullData !== true) {
    aggregationQueries.push({
      $project: {
        project: 0,
        isPublic: 0,
        isRestricted: 0
      }
    });
  }

  if (randomElements !== null) {
    aggregationQueries.push({
      $sample: {
        size: randomElements
      }
    });
  }

  const result = await asyncModelOperations.aggregate(
    entityModel,
    aggregationQueries
  );

  return result;
};

const getEntityDeps = async (project, group, deps) => {
  const aggregationQueries = [
    {
      $match: {
        $and: [
          { isRestricted: false },
          { group: group },
          {
            $or: [{ project: project }, { isPublic: true }]
          },
          {
            "metadata.hash": deps
          }
        ]
      }
    }
  ];

  const result = await asyncModelOperations.aggregate(
    entityModel,
    aggregationQueries
  );

  return result;
};

module.exports = {
  createMetadataStartup: createMetadataStartup,
  getMetadataFromBody: getMetadataFromBody,
  createEntityFromMetadata: createEntityFromMetadata,
  createEntitiesFromContainer: createEntitiesFromContainer,
  cleanupMetadata: cleanupMetadata,
  decompressZipppedEntityDeps: decompressZipppedEntityDeps,
  getFilePath: getFilePath,
  getEntityContent: getEntityContent,
  checkFileExists: checkFileExists,
  createEntity: createEntity,
  updateById: updateById,
  updateEntity: updateEntity,
  upsertTags: upsertTags,
  groupThumbnailCalcRule: groupThumbnailCalcRule,
  upsertThumb: upsertThumb,
  thumbFromContent: thumbFromContent,
  deleteEntity: deleteEntity,
  deleteEntityComplete: deleteEntityComplete,
  getEntityByIdProject: getEntityByIdProject,
  getEntitiesIdOfProjectWithGroup: getEntitiesIdOfProjectWithGroup,
  getEntityById: getEntityById,
  getEntityByHash: getEntityByHash,
  getEntityByName: getEntityByName,
  getEntitiesOfProject: getEntitiesOfProject,
  getEntitiesOfProjectWithGroup: getEntitiesOfProjectWithGroup,
  getEntityDeps: getEntityDeps,
  getEntitiesByProjectGroupTags: getEntitiesByProjectGroupTags
};
