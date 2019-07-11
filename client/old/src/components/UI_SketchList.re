Modules.require("./UI_SketchList.css");
open Utils;

module DeleteNote = [%graphql
  {|
    mutation ($noteId: String!) {
      delete_note_revision(where: {note_id: {_eq: $noteId}}) { affected_rows }
      delete_note(where: {id: {_eq: $noteId}}) { affected_rows }
    }
|}
];

module DeleteNoteComponent = ReasonApollo.CreateMutation(DeleteNote);

let component = ReasonReact.statelessComponent("UI_SketchList");

[@react.component]
let make =
    (
      ~sketches,
      ~className=?,
      ~noSketches="No sketches"->str,
      ~fetchMore=?,
      (),
    ) =>
  ReactCompat.useRecordApi({
    ...component,
    render: _self =>
      switch (sketches) {
      | [||] => <div className={Cn.unwrap(className)}> noSketches </div>
      | sketches =>
        <>
          <ul className={Cn.make(["UI_SketchList", Cn.unwrap(className)])}>
            {sketches
             ->(
                 Belt.Array.mapU((. sketch) =>
                   <li className="UI_SketchList__sketch">
                     <Router.Link
                       className="UI_SketchList__sketch--link"
                       route={Route.Note({noteId: sketch##id, data: None})}>
                       <span className="UI_SketchList__sketch--title">
                         {switch (sketch##title) {
                          | None
                          | Some("") => "untitled sketch"->str
                          | Some(title) => title->str
                          }}
                       </span>
                     </Router.Link>
                     <div className="UI_SketchList__sketch--lastEdited">
                       "last edited"->str
                       <UI_DateTime
                         date=sketch##date
                         className="UI_SketchList__sketch--time"
                       />
                     </div>
                     <DeleteNoteComponent>
                       ...{(mutation, _) =>
                         <button
                           className="btn UI_SketchList__sketch--delete"
                           onClick={_event => {
                             let continue =
                               Webapi.Dom.(
                                 Window.confirm(
                                   "Are you sure you want to delete this sketch?",
                                   window,
                                 )
                               );

                             if (continue) {
                               let deleteNoteQuery =
                                 DeleteNote.make(~noteId=sketch##id, ());

                               Js.Promise.(
                                 mutation(
                                   ~variables=deleteNoteQuery##variables,
                                   ~refetchQueries=[|"getNotes"|],
                                   (),
                                 )
                                 |> then_(_response => {
                                      Notify.info("Note was deleted.");
                                      resolve();
                                    })
                                 |> catch(err => {
                                      Notify.error("Note failed to delete.");
                                      logError(err)->resolve;
                                    })
                                 |> ignore
                               );
                             };
                           }}>
                           "Delete"->str
                         </button>
                       }
                     </DeleteNoteComponent>
                   </li>
                 )
               )
             ->React.array}
          </ul>
          {switch (fetchMore) {
           | None => ReasonReact.null
           | Some(fetchMore) =>
             <button className="btn btn-primary" onClick=fetchMore>
               "Load more sketches"->str
             </button>
           }}
        </>
      },
  });

module WithUserInfo = {
  let component =
    ReasonReact.statelessComponent("UI_SketchList.WithUserInfo");

  [@react.component]
  let make =
      (
        ~sketches:
           array({
             .
             "id": string,
             "title": option(string),
             "date": Js.Json.t,
             "owner": {
               .
               "id": string,
               "avatar": option(string),
               "username": string,
             },
           }),
        ~className=?,
        ~noSketches="No sketches"->str,
        (),
      ) =>
    ReactCompat.useRecordApi({
      ...component,
      render: _self =>
        switch (sketches) {
        | [||] => <div className={Cn.unwrap(className)}> noSketches </div>
        | sketches =>
          <ul className={Cn.make(["UI_SketchList", Cn.unwrap(className)])}>
            {sketches
             ->(
                 Belt.Array.mapU((. sketch) =>
                   <li className="UI_SketchList__sketch">
                     <Router.Link
                       className="UI_SketchList__sketch--link"
                       route={Route.Note({noteId: sketch##id, data: None})}>
                       <span className="UI_SketchList__sketch--title">
                         {switch (sketch##title) {
                          | None
                          | Some("") => "untitled sketch"->str
                          | Some(title) => title->str
                          }}
                       </span>
                     </Router.Link>
                     <UI_SketchOwnerInfo
                       owner=sketch##owner
                       noteLastEdited=sketch##date
                     />
                   </li>
                 )
               )
             ->React.array}
          </ul>
        },
    });
};

module Placeholder = {
  let component = ReasonReact.statelessComponent("UI_SketchList.Placeholder");

  [@react.component]
  let make = (~className=?, ~width=450, ()) =>
    ReactCompat.useRecordApi({
      ...component,
      render: _self =>
        <ReactContentLoader ?className height=250 width>
          <rect x="0" y="0" rx="0" ry="0" width="93" height="20" />
          <rect x="110" y="0" rx="0" ry="0" width="195" height="20" />
          <rect x="30" y="30" rx="0" ry="0" width="130" height="14" />
          <rect x="180" y="30" rx="0" ry="0" width="170" height="14" />
          <circle cx="12" cy="37" r="12" />
          <rect x="0" y="70" rx="0" ry="0" width="30" height="20" />
          <rect x="47" y="70" rx="0" ry="0" width="377" height="20" />
          <rect x="30" y="100" rx="0" ry="0" width="183" height="14" />
          <rect x="224" y="100" rx="0" ry="0" width="191" height="14" />
          <circle cx="12" cy="107" r="12" />
          <rect x="0" y="140" rx="0" ry="0" width="139" height="20" />
          <rect x="170" y="140" rx="0" ry="0" width="196" height="20" />
          <rect x="30" y="170" rx="0" ry="0" width="129" height="14" />
          <rect x="178" y="170" rx="0" ry="0" width="203" height="14" />
          <circle cx="12" cy="177" r="12" />
        </ReactContentLoader>,
    });
};
