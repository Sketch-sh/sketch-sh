import passport from "passport";
import { Strategy as GitHubStrategy, StrategyOptions, Profile } from "passport-github2";
import { VerifyFunction, VerifyCallback } from "passport-oauth2"
import { db } from "../db"
import { customAlphabet } from "nanoid";


let generateId =
  customAlphabet(
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789",
    22
  );

interface GithubProfile extends Profile {
  _json: {
    avatar_url: string
  }
}
const findUserIdFromGithubIdStmt = db.prepare(`SELECT * FROM user_identity
WHERE identity_id= ? AND identity_type= 'github' `);
const insertGithubIdentity = db.prepare(`
  INSERT INTO user_identity (user_id, identity_type, identity_id, data)
  VALUES (?, 'github', ?, ?)
  `)
const insertUser = db.prepare(`
  INSERT INTO user (id, email, name, username, avatar)
  VALUES (?1, ?2, ?3, ?4, ?5)
`)

type CreateUserArgs = {
  user_id: string,
  github_id: string,
  username: string,
  name: string | null,
  email: string | null,
  data: any,
  avatar: string
};

const verify: VerifyFunction =
  (accessToken: string, refreshToken: string, profile: GithubProfile, done: VerifyCallback) => {
    /**
     * Check if this user is already authenticated with Github
     */
    const userId = findUserIdFromGithubIdStmt.get(profile.id);

    if (!userId) {
      const email =
        profile.emails && profile.emails[0] && profile.emails[0].value;
      const newUserId = generateId();
      const args: CreateUserArgs = {
        user_id: newUserId,
        github_id: profile.id,
        username: profile.username,
        name: profile.displayName || undefined,
        email: email || undefined,
        data: { accessToken, refreshToken, profile },
        avatar: profile._json.avatar_url,
      };

      const stmt = db.transaction((args: CreateUserArgs) => {
        insertUser.run(args.user_id, args.email, args.name, args.username, args.avatar)
        insertGithubIdentity.run(args.user_id, args.github_id, args.data)
      })
      stmt(args)

      done(null, newUserId);
    } else {
      done(null, userId);
    }
  }

passport.use(
  new GitHubStrategy(
    {
      clientID: process.env.AUTH_GITHUB_CLIENT_ID,
      clientSecret: process.env.AUTH_GITHUB_CLIENT_SECRET,
      callbackURL: process.env.AUTH_GITHUB_CALLBACK_URL,
      scope: ["user:email"],
    } as StrategyOptions, verify)
);
