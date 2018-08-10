// @ts-check
require("dotenv").config();

const express = require("express");
const passport = require("passport");
const jwt = require("jsonwebtoken");
const qs = require("querystring");
const GitHubStrategy = require("passport-github2").Strategy;
const checkUserInfo = require("./checkUserInfo");
const nanoidGenerate = require("nanoid/generate");
const morgan = require("morgan");

let generateId = () =>
  nanoidGenerate(
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789",
    22
  );

passport.use(
  new GitHubStrategy(
    {
      clientID: process.env.GITHUB_CLIENT_ID,
      clientSecret: process.env.GITHUB_CLIENT_SECRET,
      callbackURL: process.env.GITHUB_CALLBACK_URL,
      scope: ["user:email"],
    },
    function(accessToken, refreshToken, profile, done) {
      (async function() {
        try {
          const userInfoResult = await checkUserInfo.githubCheckIdentity(
            profile.id
          );

          if (!userInfoResult || !userInfoResult.length) {
            const email =
              profile.emails && profile.emails[0] && profile.emails[0].value;

            const args = {
              user_id: generateId(),
              github_id: profile.id,
              username: profile.username,
              name: profile.displayName || undefined,
              email: email || undefined,
              data: { accessToken, refreshToken, profile },
              avatar: profile._json.avatar_url,
            };

            const newUser = await checkUserInfo.createGithubIdentity(args);
            const userId = newUser.data.insert_user.returning[0].id;

            done(null, userId);
          } else {
            done(null, userInfoResult[0].user_id);
          }
        } catch (error) {
          return done(error);
        }
      })();
    }
  )
);

const app = express();
app.use(morgan(process.env.NODE_ENV === "production" ? "combined" : "dev"));
app.use(passport.initialize());

app.get("/auth/github", passport.authenticate("github"), function(req, res) {
  // The request will be redirected to GitHub for authentication, so this
  // function will not be called.
});

app.get(
  "/auth/github/callback",
  passport.authenticate("github", {
    session: false,
    failureRedirect: process.env.FRONTEND_AUTH_FAILURE,
  }),
  function(req, res) {
    const userId = req.user;

    const jwtToken = jwt.sign(
      {
        userId,
        role: "user",
      },
      process.env.JWT_TOKEN
    );

    const redirectUrl =
      process.env.FRONTEND_AUTH_CALLBACK +
      "?" +
      qs.stringify({ token: jwtToken });

    res.redirect(redirectUrl);
  }
);

const jwtTokenFromHeader = req => {
  if (
    req.headers.authorization &&
    req.headers.authorization.split(" ")[0] === "Bearer"
  ) {
    return req.headers.authorization.split(" ")[1];
  }
  return null;
};

app.get("/auth/webhook", (req, res) => {
  // Extract token from request
  const token = jwtTokenFromHeader(req);

  if (!token) {
    res.json({
      "X-Hasura-Role": "public",
      "X-Hasura-Edit-Token": req.header("X-Hasura-Edit-Token") || "",
    });
    return;
  }

  jwt.verify(token, process.env.JWT_TOKEN, function(error, result) {
    if (error) {
      res.sendStatus(401);
      return;
    } else {
      res.json({
        "X-Hasura-Role": result.role,
        "X-Hasura-User-Id": result.userId,
        "X-Hasura-Edit-Token": "",
      });
      return;
    }
  });
});

module.exports = app;
