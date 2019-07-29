[@bs.val] external window: Dom.window = "window";

type message_event = {. "data": message_event_data}
and message_event_data = {
  .
  "_type": Js.Nullable.t(string),
  "payload": Container_comm.from_host,
};

type message_handler = message_event => unit;

[@bs.send]
external addMessageListener:
  (Dom.window, [@bs.as "message"] _, message_handler, bool) => unit =
  "addEventListener";

let entry_file = ref("index.re");

window->addMessageListener(
  event => {
    let data = event##data;
    switch (data##_type->Js.Nullable.toOption) {
    | Some("sketch") =>
      switch (data##payload) {
      | Comm_file_update(filename, content) =>
        Container_fs.set(~filename, ~content);

        let entry_content = Container_fs.get(~filename=entry_file^);

        switch (entry_content) {
        | None => ()
        | Some(entry_content) => Container_bundler.eval(entry_content)
        };

      | Comm_set_entry(filename) => entry_file := filename
      }
    | _ => ()
    };
  },
  false,
);

// Container_bundler.main();
