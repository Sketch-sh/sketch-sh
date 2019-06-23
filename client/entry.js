import React from "react";
import ReactDOM from "react-dom";
import { AppContainer } from "react-hot-loader";
import App from "./src/App.bs";

const render = Component => {
  ReactDOM.render(
    React.createElement(AppContainer, null, React.createElement(App)),
    document.getElementById("main")
  );
};

render(App);

if (!localStorage.getItem("login_reset")) {
  localStorage.removeItem("rtop:token");
  localStorage.removeItem("rtop:userId");
  localStorage.setItem("login_reset", "1");
}
// Webpack Hot Module Replacement API
if (module.hot) {
  module.hot.accept("./src/App.bs", () => {
    render(App);
  });
}
