const firebase = require("firebase");

var firebaseConfig = {
	apiKey: "AIzaSyDil3b8q9vwNwAzRIfuDi8XDr5zAFtbArQ",
	authDomain: "esp-cam-iot.firebaseapp.com",
	projectId: "esp-cam-iot",
	storageBucket: "esp-cam-iot.appspot.com",
	messagingSenderId: "91535917749",
	appId: "1:91535917749:web:1a0186f6dd48f2a3c774fc"
};

firebase.initializeApp(firebaseConfig);

module.exports = firebase;