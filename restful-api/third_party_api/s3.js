var AWS = require("aws-sdk");
const globalConfig = require("../config_api.js");

AWS.config = new AWS.Config();
AWS.config.region = "us-east-1";
AWS.config.accessKeyId = "AKIAJWSLLXTAUPNWMBOA"; //globalConfig.AWSConfigAccessKeyId;
AWS.config.secretAccessKey = "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"; //;globalConfig.AWSConfigSecretAccessKey;

var s3 = new AWS.S3();

function listInternal(key, complete) {
  return new Promise((resolve, reject) => {
    const key_name = "Key";
    const params = {
      Bucket: globalConfig.S3Bucket,
      Prefix: key
    };

    const p1 = s3.listObjectsV2(params).promise();
    p1.then(data => {
      let keys = [];
      const kl = key.length + 1; // +1 here includes slash /
      for (const k in data["Contents"]) {
        if (complete) {
          keys.push(data["Contents"][k][key_name]);
        } else {
          const kd = data["Contents"][k][key_name];
          const kname = kd.substring(kl, kd.length);
          if (kname.length > 0) keys.push(kname);
        }
      }
      resolve(keys);
    }).catch(err => {
      const err_string = `Cannot list ${key}`;
      console.log(err_string);
      reject(err_string);
    });
  });
}

exports.list = (key, complete) => {
  return listInternal(key, complete);
};

exports.checkObjectExists = async (key, bucket) => {
  let params = {
    Bucket: bucket,
    Key: key
  };
  try {
    await s3.headObject(params).promise();
    return true;
  } catch (err) {
    console.log("[INFO] CloudAPI : object " + params + " does not exist");
    return false;
  }
};

exports.upload = (filedata, key, bucket) => {
  const params = {
    Body: filedata,
    Bucket: bucket,
    Key: key,
    ACL: "public-read"
  };

  return s3.putObject(params).promise();
};

exports.get = (key, bucket) => {
  var params = {
    Bucket: bucket,
    Key: key
  };
  try {
    return s3.getObject(params).promise();
  } catch (err) {
    console.log(
      "[ERROR] CloudAPI-Get : object " + params + " could not be retrived"
    );
    return false;
  }
};

exports.delete = (key, bucket) => {
  var params = {
    Bucket: bucket,
    Key: key
  };

  return s3.deleteObject(params).promise();
};

exports.deleteMulti = key => {
  return new Promise((resolve, reject) => {
    let params = {
      Bucket: globalConfig.S3Bucket,
      Delete: { Objects: [] }
    };

    const p = listInternal(key, false);
    p.then(data => {
      if (data) {
        data.forEach(element => {
          params.Delete.Objects.push({ Key: key + "/" + element });
        });
        s3.deleteObjects(params).promise();
      }
      // console.log(params);
      resolve(data);
    }).catch(err => {
      const err_string = `Cannot list2 ${key}`;
      console.log(err_string);
      reject(err_string);
    });
  });
};

exports.rename = (source, dest, bucket) => {
  var paramsCopy = {
    Bucket: bucket,
    CopySource: `${bucket}/${source}`,
    Key: dest
  };

  var paramsDelete = {
    Bucket: bucket,
    Key: source
  };

  return s3
    .copyObject(paramsCopy)
    .promise()
    .then(() => s3.deleteObject(paramsDelete).promise());
};
