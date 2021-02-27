const express = require("express");
const routes = express.Router();
const firebase = require("./services/firebase");
const fs = require("fs");
const formidable = require("formidable");

// test

routes.post("/pushphoto", async (req, res) => {
  const database = firebase.database();
  const { path } = req.query;
  const ref = database.ref(path);

  console.log("Iniciando procedimento de envio de imagem.")

  const form = formidable({ multiples: true });

  let formFiles = await new Promise(function (resolve, reject) {
    form.parse(req, function (err, fields, files) {
      if (err) {
        reject(err);
        return;
      }
      resolve(files);
    });
  });

  if(formFiles){
    console.log("Formulário recebido");

    let raw = fs.readFileSync(formFiles.imageFile.path);
    let base64data = raw.toString("base64");
    image = base64data;
    
    console.log("Inicianto tentativa de envio de imagem ao firebase");
    
    await ref.push({
      base64data,
    });
    
    res.status(200).send({
      res: "ok",
    });
  } else {
    console.log("Nenhum formulário recebido");
    res.status(404).send({
      res: "Erro",
    });
  }

});

routes.get("/hello", async(req, res) => {
  res.status(200).send({
    text: "Hello World"
  });
});

module.exports = routes;