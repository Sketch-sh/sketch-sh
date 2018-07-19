module S = {
  let wrapper =
    Css.(
      [%style
        {|
          display: flex;
          position: relative;
          flex: 4;
          flex-direction: column;

          background: #000;
          color: #fff;
          font-family: monospace;
          padding: 1rem 1rem 0 1rem;
          overflow-y: auto;
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
