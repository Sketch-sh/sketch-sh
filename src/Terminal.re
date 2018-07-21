module S = {
  let wrapper =
    Css.(
      [%css
        {|
          {
            display: flex;
            position: relative;
            flex: 3;
            flex-direction: column;

            background: #000;
            color: #fff;
            font-family: monospace;
            padding: 1rem 1rem 0 1rem;
            overflow-y: auto;
          }

          &p {
            margin: 0;
            padding: 0;
          }
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
