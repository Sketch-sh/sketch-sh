[@bs.config {jsx: 3}];

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
/*
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
 */
module Save = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make:
    (~className: string=?, ~children: React.element) => React.element =
    "FiSave";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className: string=?, children) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className, ()),
        children,
      );
    };
  };
};
/*
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
 */
module Loader = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make:
    (~className: string=?, ~children: React.element) => React.element =
    "FiLoader";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className: string=?, children) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className, ()),
        children,
      );
    };
  };
};
/*
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
 */
module Home = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make:
    (~className: string=?, ~children: React.element) => React.element =
    "FiHome";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className: string=?, children) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className, ()),
        children,
      );
    };
  };
};
/*
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
 */
module Github = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make:
    (~className: string=?, ~children: React.element) => React.element =
    "Github";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className: string=?, children) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className, ()),
        children,
      );
    };
  };
};
/*
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
 */
module GitBranch = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make:
    (~className: string=?, ~children: React.element) => React.element =
    "FiGitBranch";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className: string=?, children) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className, ()),
        children,
      );
    };
  };
};
/*
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
 */
module Terminal = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make:
    (~className: string=?, ~children: React.element) => React.element =
    "FiTerminal";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className: string=?, children) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className, ()),
        children,
      );
    };
  };
};
/*
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
 */
module PlusCircle = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make:
    (~className: string=?, ~children: React.element) => React.element =
    "FiPlusCircle";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className: string=?, children) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className, ()),
        children,
      );
    };
  };
};
/*
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
 */
module FilePlus = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make:
    (~className: string=?, ~children: React.element) => React.element =
    "FiFilePlus";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className: string=?, children) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className, ()),
        children,
      );
    };
  };
};
/*
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
 */
module Plus = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make:
    (~className: string=?, ~children: React.element) => React.element =
    "FiPlus";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className: string=?, children) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className, ()),
        children,
      );
    };
  };
};
/*
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
 */
module Code = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make:
    (~className: string=?, ~children: React.element) => React.element =
    "FiCode";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className: string=?, children) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className, ()),
        children,
      );
    };
  };
};
/*
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
 */
module Edit2 = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make:
    (~className: string=?, ~children: React.element) => React.element =
    "FiEdit2";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className: string=?, children) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className, ()),
        children,
      );
    };
  };
};
/*
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
 */
module Trash2 = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make:
    (~className: string=?, ~children: React.element) => React.element =
    "FiTrash2";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className: string=?, children) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className, ()),
        children,
      );
    };
  };
};
/*
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
 */
module RefreshCw = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make:
    (~className: string=?, ~children: React.element) => React.element =
    "FiRefreshCw";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className: string=?, children) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className, ()),
        children,
      );
    };
  };
};
/*
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
 */
module Package = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make:
    (~className: string=?, ~children: React.element) => React.element =
    "FiPackage";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className: string=?, children) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className, ()),
        children,
      );
    };
  };
};
/*
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
 */
module Play = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make:
    (~className: string=?, ~children: React.element) => React.element =
    "FiPlay";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className: string=?, children) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className, ()),
        children,
      );
    };
  };
};
/*
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
 */
module Link = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make:
    (~className: string=?, ~children: React.element) => React.element =
    "FiLink";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className: string=?, children) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className, ()),
        children,
      );
    };
  };
};
/*
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
 */
module RefreshCCW = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make:
    (~className: string=?, ~children: React.element) => React.element =
    "FiRefreshCcw";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className: string=?, children) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className, ()),
        children,
      );
    };
  };
};
/*
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
 */
module FiXCircle = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make:
    (~className: string=?, ~children: React.element) => React.element =
    "FiXCircle";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className: string=?, children) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className, ()),
        children,
      );
    };
  };
};
/*
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
 */
module FiRefreshCw = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make:
    (~className: string=?, ~children: React.element) => React.element =
    "FiRefreshCw";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className: string=?, children) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className, ()),
        children,
      );
    };
  };
};
