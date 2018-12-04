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

module PlusCircle = {
  [@bs.module "react-icons/fi"]
  external reactClass: ReasonReact.reactClass = "FiPlusCircle";

  let make = (~className="", children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props={"className": className},
      children,
    );
};

module FilePlus = {
  [@bs.module "react-icons/fi"]
  external reactClass: ReasonReact.reactClass = "FiFilePlus";

  let make = (~className="", children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props={"className": className},
      children,
    );
};

module Plus = {
  [@bs.module "react-icons/fi"]
  external reactClass: ReasonReact.reactClass = "FiPlus";

  let make = (~className="", children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props={"className": className},
      children,
    );
};

module Code = {
  [@bs.module "react-icons/fi"]
  external reactClass: ReasonReact.reactClass = "FiCode";

  let make = (~className="", children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props={"className": className},
      children,
    );
};

module Edit2 = {
  [@bs.module "react-icons/fi"]
  external reactClass: ReasonReact.reactClass = "FiEdit2";

  let make = (~className="", children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props={"className": className},
      children,
    );
};

module Trash2 = {
  [@bs.module "react-icons/fi"]
  external reactClass: ReasonReact.reactClass = "FiTrash2";

  let make = (~className="", children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props={"className": className},
      children,
    );
};

module RefreshCw = {
  [@bs.module "react-icons/fi"]
  external reactClass: ReasonReact.reactClass = "FiRefreshCw";

  let make = (~className="", children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props={"className": className},
      children,
    );
};

module Package = {
  [@bs.module "react-icons/fi"]
  external reactClass: ReasonReact.reactClass = "FiPackage";

  let make = (~className="", children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props={"className": className},
      children,
    );
};

module Play = {
  [@bs.module "react-icons/fi"]
  external reactClass: ReasonReact.reactClass = "FiPlay";

  let make = (~className="", children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props={"className": className},
      children,
    );
};

module Link = {
  [@bs.module "react-icons/fi"]
  external reactClass: ReasonReact.reactClass = "FiLink";

  let make = (~className="", children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props={"className": className},
      children,
    );
};

module RefreshCCW = {
  [@bs.module "react-icons/fi"]
  external reactClass: ReasonReact.reactClass = "FiRefreshCcw";

  let make = (~className="", children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props={"className": className},
      children,
    );
};

module FiXCircle = {
  [@bs.module "react-icons/fi"]
  external reactClass: ReasonReact.reactClass = "FiXCircle";

  let make = (~className="", children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props={"className": className},
      children,
    );
};

module FiRefreshCw = {
  [@bs.module "react-icons/fi"]
  external reactClass: ReasonReact.reactClass = "FiRefreshCw";

  let make = (~className="", children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass,
      ~props={"className": className},
      children,
    );
};
