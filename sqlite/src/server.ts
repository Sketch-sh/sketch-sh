import "./startup"
import express from "express"
import morgan from 'morgan'
import { router } from "./v1/v1";
import passport from "passport"
import qs from "querystring"
import jwt from "jsonwebtoken"
import "./middlewares/passport"

const app = express();
app.use(morgan(process.env.NODE_ENV === "production" ? "combined" : "dev"));
app.use(passport.initialize())

const FRONTEND_AUTH_FAILURE = process.env.FRONTEND_URL + "/auth/failure";
const FRONTEND_AUTH_CALLBACK = process.env.FRONTEND_URL + "/auth/callback";

app.get("/api/auth/github", passport.authenticate("github"))
app.get("/api/auth/github/callback",
  passport.authenticate("github", {
    session: false,
    failureRedirect: FRONTEND_AUTH_FAILURE,
  }),
  (req, res) => {
    const userId = req.user;

    const jwtToken = jwt.sign(
      {
        userId,
        role: "user",
      },
      process.env.SECRET_JWT_TOKEN,
      {
        algorithm: "HS256"
      }
    );

    const redirectUrl =
      FRONTEND_AUTH_CALLBACK + "?" + qs.stringify({ token: jwtToken });
    res.redirect(redirectUrl);
  }


)
app.use("/api/v1", router)

export default app
