let umd_pathname: Js.Dict.t(string) = [%bs.raw
  {|
  {
    "react": "umd/react.development.min.js",
    "react-dom": "umd/react-dom.development.min.js"
  }
|}
];

let package_map: Js.Dict.t(string) = [%bs.raw
  {|
  {
    "crypto": "crypto-browserify",
  }
|}
];
