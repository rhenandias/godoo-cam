const express = require("express");
const routes = express.Router();
const firebase = require("./services/firebase");
const fs = require("fs");
const formidable = require("formidable");

/*
  Procedimento de escrita da API:
  
  A API utilizada deve ser capaz de:
  - Receber um arquivo via formulário multform/form-data
  - Salvar o arquivo recebido, e acessa-lo
  - Enviar o arquivo para o banco de dados

  Corpo da requisição enviada pela ESP:

    POST <api_url> HTTP/1.1
    Host: <server_name>
    Content-Length:  <query_lenght>
    Content-Type: multipart/form-data; boundary=EspCamIOT
    
    --EspCamIOT
    Content-Disposition: form-data; name=\"imageFile\"; filename=\"esp32-cam.jpg\"
    Content-Type: image/jpeg
    <image_data>
    --EspCamIOT--

  Procedimento adotado por mim:
    
    Para esse teste, utilizei o firebase como banco de dados, e uma API em nodejs
    Assim, utilizei a biblioteca "formidable" para receber o arquivo em NodeJS
    Para acessar o arquivo utilizei a biblioteca padrão "fs"
    Como a imagem lida é uma sequência de bytes, para enviar ao banco converti primeiro em Base64
    Após converter em Base64 os dados são enviados ao banco de dados
    Portanto, as imagens são registradas no banco de dados como Base64
    
*/

routes.post("/pushphoto", async (req, res) => {
  const { path } = req.query;

  const database = firebase.database();
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