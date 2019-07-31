open Frame_comm;

let entry_file = ref("index.re");

window->addMessageListener(
  event => {
    let data = event##data;
    switch (data##_type->Js.Nullable.toOption) {
    | Some("sketch") =>
      switch (data##payload) {
      | Container_comm.Comm_file_update(filename, content) =>
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

to_host(Comm_ready);

// ConsoleFeed.hook([%raw "window.console"], log =>
//   to_host(Comm_send_log(log))
// );

%bs.raw
{|
if (module.hot) {
  module.hot.accept("./Container.bs.js", () => {
    location.reload();
  });
}
|};
