open Utils;

type state = {
  value: string,
  widgets: array(Editor_Types.Widget.t),
};

type action =
  | UpdateValue(string)
  | UpdateWidgets(array(Editor_Types.Widget.t))
  | Run;

let component = ReasonReact.reducerComponent("Embed_Editor");

let make = (~initialValue="", _children) => {
  ...component,
  initialState: () => {value: initialValue, widgets: [||]},
  reducer: (action, state) =>
    switch (action) {
    | UpdateValue(value) => ReasonReact.Update({...state, value})
    | UpdateWidgets(widgets) => ReasonReact.Update({...state, widgets})
    | Run =>
      ReasonReact.SideEffects(
        (
          ({state, send, onUnmount}) => {
            let cancelToken =
              Toplevel_Consumer.executeEmbed(
                ~lang=RE,
                ~code=state.value,
                fun
                | Belt.Result.Ok(result) => {
                    let widgets =
                      Editor_Blocks_Utils.executeResultToWidget(result);
                    send(UpdateWidgets(widgets));
                  }
                | Belt.Result.Error(message) => (),
              );
            onUnmount(() => Toplevel_Consumer.cancel(cancelToken));
          }
        ),
      )
    },
  render: ({state, send}) =>
    <>
      <section id="editor">
        <Editor_CodeBlock
          value={state.value}
          firstLineNumber=1
          focused={Some(FcTyp_EditorFocus)}
          widgets={state.widgets}
          onChange={(value, _) => send(UpdateValue(value))}
        />
      </section>
      <footer id="footer">
        <span className="footer-cell">
          <span> "Powered by "->str </span>
          <strong> "Sketch"->str </strong>
        </span>
        <span className="footer_spacing" />
        <button className="footer_run" onClick={_ => send(Run)}>
          "Run"->str
        </button>
      </footer>
    </>,
};
