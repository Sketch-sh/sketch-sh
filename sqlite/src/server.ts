import "./startup"
import express from "express"
import morgan from 'morgan'
import { router } from "./v1/v1";

const app = express();
app.use(morgan(process.env.NODE_ENV === "production" ? "combined" : "dev"));
app.use("/v1", router)

const PORT = parseInt(process.env.PORT) || 3002;
app.listen(PORT, () => {
  console.log(`server started at http://localhost:${PORT}`);
});
