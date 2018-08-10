const webpack = require("webpack");
const HtmlWebpackPlugin = require("html-webpack-plugin");
const path = require("path");
const postcssPresetEnv = require("postcss-preset-env");
const UglifyJsPlugin = require("uglifyjs-webpack-plugin");
const MiniCssExtractPlugin = require("mini-css-extract-plugin");
const OptimizeCSSAssetsPlugin = require("optimize-css-assets-webpack-plugin");

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
    disableHostCheck: true,
    proxy: {
      "/graphql": "http://localhost:8081/v1alpha1",
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
          file.endsWith(".worker.js") || file.endsWith("Worker_Index.bs.js"),
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
    ],
  },
};

if (!isProd) {
  base.plugins = [...base.plugins, new webpack.HotModuleReplacementPlugin()];
}

module.exports = base;
