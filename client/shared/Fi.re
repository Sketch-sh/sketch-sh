[@bs.config {jsx: 3}];

module Save = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make: (~className: string=?) => React.element = "FiSave";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className=?) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className?, ()),
      );
    };
  };
};

module Loader = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make: (~className: string=?) => React.element = "FiLoader";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className=?) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className?, ()),
      );
    };
  };
};

module Home = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make: (~className: string=?) => React.element = "FiHome";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className=?) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className?, ()),
      );
    };
  };
};

module Github = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make: (~className: string=?) => React.element = "FiGithub";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className=?) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className?, ()),
      );
    };
  };
};

module GitBranch = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make: (~className: string=?) => React.element = "FiGitBranch";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className=?) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className?, ()),
      );
    };
  };
};

module Terminal = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make: (~className: string=?) => React.element = "FiTerminal";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className=?) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className?, ()),
      );
    };
  };
};

module PlusCircle = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make: (~className: string=?) => React.element = "FiPlusCircle";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className=?) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className?, ()),
      );
    };
  };
};

module FilePlus = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make: (~className: string=?) => React.element = "FiFilePlus";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className=?) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className?, ()),
      );
    };
  };
};

module Plus = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make: (~className: string=?) => React.element = "FiPlus";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className=?) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className?, ()),
      );
    };
  };
};

module Code = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make: (~className: string=?) => React.element = "FiCode";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className=?) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className?, ()),
      );
    };
  };
};

module Edit2 = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make: (~className: string=?) => React.element = "FiEdit2";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className=?) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className?, ()),
      );
    };
  };
};

module Trash2 = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make: (~className: string=?) => React.element = "FiTrash2";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className=?) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className?, ()),
      );
    };
  };
};

module RefreshCw = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make: (~className: string=?) => React.element = "FiRefreshCw";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className=?) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className?, ()),
      );
    };
  };
};

module Package = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make: (~className: string=?) => React.element = "FiPackage";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className=?) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className?, ()),
      );
    };
  };
};

module Play = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make: (~className: string=?) => React.element = "FiPlay";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className=?) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className?, ()),
      );
    };
  };
};

module Link = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make: (~className: string=?) => React.element = "FiLink";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className=?) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className?, ()),
      );
    };
  };
};

module RefreshCCW = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make: (~className: string=?) => React.element = "FiRefreshCcw";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className=?) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className?, ()),
      );
    };
  };
};

module FiXCircle = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make: (~className: string=?) => React.element = "FiXCircle";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className=?) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className?, ()),
      );
    };
  };
};

module FiRefreshCw = {
  [@bs.module "react-icons/fi"] [@react.component]
  external make: (~className: string=?) => React.element = "FiRefreshCw";

  module Jsx2 = {
    let component = ReasonReact.statelessComponent(__MODULE__);
    let make = (~className=?) => {
      ReasonReactCompat.wrapReactForReasonReact(
        make,
        makeProps(~className?, ()),
      );
    };
  };
};
