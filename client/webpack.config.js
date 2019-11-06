//@ts-check

const webpack = require("webpack");
const HtmlWebpackPlugin = require("html-webpack-plugin");
const path = require("path");
const UglifyJsPlugin = require("uglifyjs-webpack-plugin");
const MiniCssExtractPlugin = require("mini-css-extract-plugin");
const OptimizeCSSAssetsPlugin = require("optimize-css-assets-webpack-plugin");
const Stylish = require("webpack-stylish");

const postcssPresetEnv = require("postcss-preset-env");
const postcssNormalize = require("postcss-normalize");

const outputDir = path.join(__dirname, "build/");

const isProd = process.env.NODE_ENV === "production";

/** @type webpack.Configuration */
const base = {
  entry: {
    app: ["react-hot-loader/patch", "./entry.js"],
    container: ["./src/container/Container.bs.js"],
  },
  mode: isProd ? "production" : "development",
  devServer: {
    hot: true,
    contentBase: path.join(__dirname, "public"),
    host: "0.0.0.0",
    port: 3000,
    historyApiFallback: true,
    disableHostCheck: true,
    proxy: {
      "/graphql": "http://localhost:8080/v1alpha1",
      "/api": "http://localhost:3001/",
    },
    stats: "none",
  },
  output: {
    path: outputDir,
    publicPath: "/",
    filename: isProd ? "[name].[chunkhash].js" : "[name].js",
    globalObject: "this",
  },
  node: {
    fs: "empty",
    child_process: "empty",
  },
  resolve: {
    extensions: [".ts", ".tsx", ".mjs", ".js", ".json"],
  },
  plugins: [
    new HtmlWebpackPlugin({
      template: path.join(__dirname, "src", "index.html"),
      chunks: ["app"],
      filename: "index.html",
    }),
    new HtmlWebpackPlugin({
      template: path.join(__dirname, "src", "container.html"),
      chunks: ["container"],
      filename: "container.html",
    }),
    new MiniCssExtractPlugin({
      filename: isProd ? "[name].[contenthash].css" : "[name].css",
      chunkFilename: isProd ? "[id].[contenthash].css" : "[id].css",
    }),
    new Stylish(),
  ],
  optimization: {
    minimizer: [
      new UglifyJsPlugin({
        cache: true,
        parallel: true,
        sourceMap: false,
      }),
      new OptimizeCSSAssetsPlugin({}),
    ],
  },
  module: {
    rules: [
      {
        test: /\.tsx?$/,
        use: [
          {
            loader: "ts-loader",
            options: {
              transpileOnly: true,
            },
          },
        ],
      },
      {
        test: file =>
          file.endsWith(".js") &&
          !file.endsWith(".bs.js") &&
          !file.endsWith(".gen.js") &&
          !file.endsWith(".worker.js"),
        include: path.join(__dirname, "src"),
        use: { loader: "babel-loader" },
      },
      // {
      //   test: file => file.endsWith(".worker.js"),
      //   exclude: /public/,
      //   use: { loader: "worker-loader" },
      // },
      {
        test: /\.css$/,
        use: [
          isProd ? MiniCssExtractPlugin.loader : "style-loader",
          {
            loader: "css-loader",
            options: {
              importLoaders: 1,
            },
          },
          {
            loader: "postcss-loader",
            options: {
              ident: "postcss",
              plugins: () => [
                postcssNormalize(),
                postcssPresetEnv({
                  stage: 3,
                  features: {
                    autoprefixer: isProd,
                  },
                }),
              ],
            },
          },
        ],
      },
      {
        test: /\.(png|jpg|gif)$|toplevel.js$/,
        loader: "file-loader?name=[name].[hash].[ext]",
      },
    ],
  },
};

if (!isProd) {
  base.plugins = [
    ...base.plugins,
    // new webpack.HotModuleReplacementPlugin()
  ];
}

if (process.env.ANALYZE) {
  base.devtool = "source-map";
}

module.exports = base;
