external _unsafeToJson: Js.Dict.t(Js.Json.t) => Js.Json.t = "%identity";
let queryJson = req => req->Express.Request.query->_unsafeToJson;
