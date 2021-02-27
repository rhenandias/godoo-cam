const express = require("express");
const cors = require("cors");
const dotenv = require("dotenv");

dotenv.config({ path: ".env" });

const app = express();
app.use(express.json());
app.use(cors());
app.use("/api", require("./src/routes"));

let port = process.env.PORT;
if (port == null || port == "") {
  port = 4000;
}

app.listen(port);