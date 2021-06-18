import expressJwt from "express-jwt"

export const jwt = expressJwt({
  secret: process.env.SECRET_JWT_TOKEN,
  algorithms: ["HS256"]
}) 
