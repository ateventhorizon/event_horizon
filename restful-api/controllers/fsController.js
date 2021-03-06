const cloudApi = require("../third_party_api/s3");

exports.cloudStorageFileUpload = (body, key, bucket) => {
  return cloudApi.upload(body, key, bucket);
};

exports.cloudStorageFileGet = (key, bucket) => {
  return cloudApi.get(key, bucket);
};

exports.cloudStorageEntityGet = key => {
  return cloudApi.get(key, "eventhorizonentities");
};

exports.cloudStorageRename = (source, dest, bucket) => {
  return cloudApi.rename(source, dest, bucket);
};

exports.cloudStorageDelete = (key, bucket) => {
  return cloudApi.delete(key, bucket);
};

exports.cloudStorageCheckExists = async (key, bucket) => {
  return await cloudApi.checkObjectExists(key, bucket);
};

exports.cloudStorageGetFilenameAndDuplicateIfExists = async (
  key,
  bucket,
  filename
) => {
  let finalName = key;
  const result = await cloudApi.checkObjectExists(key, bucket);
  if (result == true) {
    // Now the file has been found so add the classic ugly orrible _N appendix
    const str = key;
    let n = str.lastIndexOf(".");
    if (n == -1) n = str.length;
    const d = new Date();
    finalName =
      str.substring(0, n) + "_" + d.getTime() + str.substring(n, str.length);
    filename["changed"] = true;
  } else {
    filename["changed"] = false;
  }
  filename["name"] = finalName;
};

// exports.cloudStorageDeleteMulti = (key) => {
// 	return cloudApi.deleteMulti( key );
// }

// exports.cloudStorageFileList = ( key, completed ) => {
//     return cloudApi.list( key, completed );
// }

exports.writeFile = (res, entity, data) => {
  res
    .status(200)
    .set({
      "Content-Type": entity.contentType,
      "Content-Last-Modified": entity.lastUpdatedDate,
      "ETag": entity.hash,
      "Content-Length": data.length
    })
    .send(data);
};

exports.writeError = (res, number, err, message) => {
  console.log("[ERROR-CAUGHT]");
  console.log(err, err.stack);
  res.status(number).send(message);
};
