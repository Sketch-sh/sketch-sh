//@ts-check

const webpack = require("webpack");
const HtmlWebpackPlugin = require("html-webpack-plugin");
const CopyWebpackPlugin = require('copy-webpack-plugin');
const path = require("path");
const UglifyJsPlugin = require("uglifyjs-webpack-plugin");
const MiniCssExtractPlugin = require("mini-css-extract-plugin");
const OptimizeCSSAssetsPlugin = require("optimize-css-assets-webpack-plugin");

const postcssPresetEnv = require("postcss-preset-env");

const outputDir = path.join(__dirname, "build/");

const isProd = process.env.NODE_ENV === "production";

/** @type webpack.Configuration */
const base = {
  entry: {
    app: ["react-hot-loader/patch", "./entry.js"],
    embedApp: ["react-hot-loader/patch", "./embedApp.js"],
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
      "/graphql": "http://localhost:8080/v1",
      "/api": "http://localhost:3002",
    },
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
  plugins: [
    new HtmlWebpackPlugin({
      template: path.join(__dirname, "src", "index.html"),
      chunks: ["app"],
      filename: "index.html",
    }),
    new CopyWebpackPlugin([
      { from: 'src/404.html' },
    ]),
    new HtmlWebpackPlugin({
      template: path.join(__dirname, "src", "embed.html"),
      chunks: ["embedApp"],
      filename: "embed.html",
    }),
    new MiniCssExtractPlugin({
      filename: isProd ? "[name].[contenthash].css" : "[name].css",
      chunkFilename: isProd ? "[id].[contenthash].css" : "[id].css",
    }),
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
        test: file =>
          file.endsWith(".worker.js") ||
          file.endsWith("Worker_Index.bs.js") ||
          file.endsWith("Toplevel_Worker.bs.js"),
        exclude: ["public"],
        use: { loader: "worker-loader" },
      },
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
                postcssPresetEnv({
                  stage: 3,
                  features: {
                    autoprefixer: isProd,
                    "custom-properties": false,
                    "color-mod-function": true,
                    "nesting-rules": true,
                    "custom-media-queries": {
                      extensions: {
                        "--sm": "screen and (min-width: 35.5rem)",
                        "--md": "screen and (min-width: 48rem)",
                        "--lg": "screen and (min-width: 64rem)",
                        "--xl": "screen and (min-width: 80rem)",
                      },
                    },
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
  base.plugins = [...base.plugins, new webpack.HotModuleReplacementPlugin()];
}

if (process.env.ANALYZE) {
  base.devtool = "source-map";
}

// const embed = {
//   entry: "./embed.js",
//   output: {
//     path: path.join(__dirname, "public/"),
//     library: "Sketch",
//     libraryTarget: "umd",
//   },
//   mode: isProd ? "production" : "development",
//   plugins: [
//     new webpack.DefinePlugin({
//       SERVICE_URL: JSON.stringify(
//         isProd ? "https://sketch.sh" : "http://localhost:3000"
//       ),
//     }),
//   ],
// };

module.exports = base;
