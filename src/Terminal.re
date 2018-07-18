open Utils;

module S = {
  let wrapper =
    Css.(
      [%style
        {|
          display: flex;
          position: relative;
          flex: 1;
          flex-direction: column;

          background: #000;
          color: #fff;
          font-family: monospace;
          padding: 1rem;
        |}
      ]
      |. style
    );
};

let component = ReasonReact.statelessComponent("Terminal");

let make = _children => {
  ...component,
  render: _self => S.(<div className=wrapper> <Terminal_Content /> </div>),
};
