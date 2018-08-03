open Utils;

let authDomain =
  env == "production" ? "http://localhost:3001" : "http://localhost:3001";
