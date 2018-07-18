const webpack = require("webpack");
const HtmlWebpackPlugin = require("html-webpack-plugin");
const path = require("path");
const outputDir = path.join(__dirname, "build/");

const isProd = process.env.NODE_ENV === "production";

const base = {
  entry: ["react-hot-loader/patch", "./entry.js"],
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
  },
  plugins: [
    new HtmlWebpackPlugin({
      template: path.join(__dirname, "src/index.html"),
    }),
  ],
};

if (!isProd) {
  base.plugins = [...base.plugins, new webpack.HotModuleReplacementPlugin()];
}

module.exports = base;
