/* react-icons: feather icon */
module IconContext = {
  [@bs.deriving abstract]
  type iconContext = {
    [@bs.as "Provider"]
    provider: ReasonReact.reactClass,
  };
  [@bs.module "react-icons"] external iconContext: iconContext = "IconContext";

  module Provider = {
    let reactClass = providerGet(iconContext);

    let make = (~value, children) =>
      ReasonReact.wrapJsForReason(
        ~reactClass,
        ~props={"value": value},
        children,
      );
  };
};

module Save = {
  [@bs.module "react-icons/fi"]
  external reactClass: ReasonReact.reactClass = "FiSave";

  let make = (~className="", children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props={"className": className},
      children,
    );
};

module Loader = {
  [@bs.module "react-icons/fi"]
  external reactClass: ReasonReact.reactClass = "FiLoader";

  let make = (~className="", children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props={"className": className},
      children,
    );
};

module Home = {
  [@bs.module "react-icons/fi"]
  external reactClass: ReasonReact.reactClass = "FiHome";

  let make = (~className="", children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props={"className": className},
      children,
    );
};

module Github = {
  [@bs.module "react-icons/fi"]
  external reactClass: ReasonReact.reactClass = "FiGithub";

  let make = (~className="", children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props={"className": className},
      children,
    );
};

module GitBranch = {
  [@bs.module "react-icons/fi"]
  external reactClass: ReasonReact.reactClass = "FiGitBranch";

  let make = (~className="", children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props={"className": className},
      children,
    );
};

module Terminal = {
  [@bs.module "react-icons/fi"]
  external reactClass: ReasonReact.reactClass = "FiTerminal";

  let make = (~className="", children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props={"className": className},
      children,
    );
};
