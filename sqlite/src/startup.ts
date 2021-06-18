/**
 * This file is for setting up server start up checks
 * Throws if start up conditions are not sastified
 */
import { config } from "dotenv"
/*
 * Setup environment variables
 */
config();

/**
 * Required environment variables
 */
[
  "AUTH_GITHUB_CLIENT_ID",
  "AUTH_GITHUB_CLIENT_SECRET",
  "AUTH_GITHUB_CALLBACK_URL",
  "FRONTEND_URL",
  "SECRET_JWT_TOKEN",
].forEach((env) => {
  if (typeof process.env[env] === "undefined") {

    console.error("Missing environment varible", env)
    process.exit(1)
  }
})

