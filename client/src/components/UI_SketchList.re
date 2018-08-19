Modules.require("./UI_SketchList.css");
open Utils;

let component = ReasonReact.statelessComponent("UI_SketchList");

let make =
    (~sketches, ~className=?, ~noSketches="No sketches"->str, _children) => {
  ...component,
  render: _self =>
    switch (sketches) {
    | [||] => <div className=(Cn.unwrap(className))> noSketches </div>
    | sketches =>
      <ul className=(Cn.make(["UI_SketchList", Cn.unwrap(className)]))>
        ...sketches
           ->(
               Belt.Array.mapU((. sketch) =>
                 <li className="UI_SketchList__sketch">
                   <Router.Link
                     className="UI_SketchList__sketch--link"
                     route=(Route.Note({noteId: sketch##id, data: None}))>
                     <span className="UI_SketchList__sketch--title">
                       (
                         switch (sketch##title) {
                         | None
                         | Some("") => "untitled sketch"->str
                         | Some(title) => title->str
                         }
                       )
                     </span>
                     <span className="UI_SketchList__sketch--dash">
                       " - "->str
                     </span>
                     ("/s/" ++ sketch##id)->str
                   </Router.Link>
                   <div className="UI_SketchList__sketch--lastEdited">
                     "last edited"->str
                     <UI_DateTime
                       date=sketch##date
                       className="UI_SketchList__sketch--time"
                     />
                   </div>
                 </li>
               )
             )
      </ul>
    },
};

module WithUserInfo = {
  let component =
    ReasonReact.statelessComponent("UI_SketchList.WithUserInfo");

  let make =
      (~sketches, ~className=?, ~noSketches="No sketches"->str, _children) => {
    ...component,
    render: _self =>
      switch (sketches) {
      | [||] => <div className=(Cn.unwrap(className))> noSketches </div>
      | sketches =>
        <ul className=(Cn.make(["UI_SketchList", Cn.unwrap(className)]))>
          ...sketches
             ->(
                 Belt.Array.mapU((. sketch) =>
                   <li className="UI_SketchList__sketch">
                     <Router.Link
                       className="UI_SketchList__sketch--link"
                       route=(Route.Note({noteId: sketch##id, data: None}))>
                       <span className="UI_SketchList__sketch--title">
                         (
                           switch (sketch##title) {
                           | None
                           | Some("") => "untitled sketch"->str
                           | Some(title) => title->str
                           }
                         )
                       </span>
                       <span className="UI_SketchList__sketch--dash">
                         " - "->str
                       </span>
                       ("/s/" ++ sketch##id)->str
                     </Router.Link>
                     <UI_SketchOwnerInfo
                       owner=sketch##owner
                       noteLastEdited=sketch##date
                     />
                   </li>
                 )
               )
        </ul>
      },
  };
};
