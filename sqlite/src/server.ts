import express from "express"
import { z } from "zod";
import {db} from "./db" 
import morgan from 'morgan'

const app = express();
app.use(morgan(process.env.NODE_ENV === "production" ? "combined" : "dev"));
const PORT = parseInt(process.env.PORT) || 3002;

const ValidateUser = z.object({ userId: z.string() });
app.get("/users/:userId", (req, res) => {
  const { userId } = ValidateUser.parse(req.params);
  const userRow = db.prepare("SELECT id, name, email, avatar FROM user WHERE id = ?").get(userId);
  if (!userRow) {
    res.status(404).json({ error: "NOT_FOUND" });
  }
  res.status(200).json({
    data: userRow,
  });
});

app.get("/sketches/recentSketches", (req, res) => {
  const { userId } = ValidateUser.parse(req.query);
  const sketches = db
    .prepare(
      `
  SELECT n.id, n.title, n.updated_at as date FROM note n
  WHERE n.user_id = ?
  ORDER BY n.updated_at
  LIMIT 5;
`
    )
    .all(userId);
  res.json({
    data: sketches,
  });
});

const CommunitySketches = z.object({ userId: z.string().optional() });
app.get("/sketches/communitySketches", (req, res) => {
  // Default value to avoid n.user_id <> NULL (which means no rows)
  const { userId = "" } = CommunitySketches.parse(req.query);
  const sketches = db
    .prepare(
      `
  SELECT 
    n.id, 
    n.title, 
    n.created_at as date, 
    user.id as userId,
    user.avatar as userAvatar,
    user.username as userName
  FROM note n
  INNER JOIN user ON user_id = user.id
  WHERE n.user_id <> ? AND n.user_id <> 'anonymous'
  ORDER BY n.created_at
  LIMIT 5;
`
    )
    .all(userId);
  res.json({
    data: sketches,
  });
});

// mutation ($noteId: String!, $data: jsonb!, $title: String!) {
//   mutate: update_note(where: {id: {_eq: $noteId}}, _set: {title: $title, data: $data}) {
//     affected_rows
//     returning {
//       __typename
//       id
//       title
//       data
//       updated_at
//       user_id
//       fork_from
//     }
//     __typename
//   }
// }

const ValidateNote = z.object({ noteId: z.string() });

// query getNoteById
app.get("/sketches/:noteId", (req, res) => {
  const { noteId } = ValidateNote.parse(req.params);
  const note = db
    .prepare(
      `
  SELECT 
    id, title, data, updated_at, user_id, fork_from, 
    t.token as noteEditToken
  FROM note
  LEFT JOIN note_edit_token as t ON note.id = t.note_id
  WHERE id = ?
  `
    )
    .get(noteId);
  if (!note) {
    res.status(404).json({ error: "NOT_FOUND" });
  }

  res.status(200).json({
    data: note,
  });
});

// start the Express server
app.listen(PORT, () => {
  console.log(`server started at http://localhost:${PORT}`);
});
