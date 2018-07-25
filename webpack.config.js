const webpack = require("webpack");
const HtmlWebpackPlugin = require("html-webpack-plugin");
const path = require("path");
const outputDir = path.join(__dirname, "build/");

const isProd = process.env.NODE_ENV === "production";
const baseFolder = process.env.EDITOR === "1" ? "src_editor" : "src";

const base = {
  entry: ["react-hot-loader/patch", "./" + baseFolder + "/entry.js"],
  mode: isProd ? "production" : "development",
  devServer: {
    hot: true,
    contentBase: path.join(__dirname, "public"),
    host: "0.0.0.0",
    port: 3000,
    historyApiFallback: true,
  },
  output: {
    path: outputDir,
    publicPath: "/",
    filename: "[name].js",
    globalObject: "this",
  },
  node: {
    fs: "empty",
    child_process: "empty",
  },
  plugins: [
    new HtmlWebpackPlugin({
      template: path.join(__dirname, baseFolder, "index.html"),
    }),
  ],
  module: {
    rules: [
      {
        test: file =>
          file.endsWith(".worker.js") || file.endsWith("Worker_Index.bs.js"),
        use: { loader: "worker-loader" },
      },
      {
        test: /\.css$/,
        use: ["style-loader", "css-loader"],
      },
      {
        test: file => file.endsWith(".js") && !file.endsWith(".bs.js"),
        include: /src_editor/,
        exclude: /node_modules/,
        use: {
          loader: "babel-loader",
          options: {
            presets: [
              ["@babel/preset-env", { modules: false }],
              "@babel/preset-react",
            ],
            plugins: ["react-hot-loader/babel"],
            babelrc: false,
          },
        },
      },
    ],
  },
};

if (!isProd) {
  base.plugins = [...base.plugins, new webpack.HotModuleReplacementPlugin()];
}

module.exports = base;
