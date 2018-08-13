Modules.require("./Editor_NoteOwnerInfo.css");
open Utils;
let component = ReasonReact.statelessComponent("Edit_NoteOwnerInfo");

let make = (~noteOwner as user, ~noteLastEdited=?, _children) => {
  ...component,
  render: _self =>
    <div className="EditorNoteOwnerInfo">
      <Router.Link route=(Route.User(optionToEmptyString(user##username)))>
        (
          user##avatar
          =>> (
            avatar =>
              <img
                src=avatar
                width="24"
                height="24"
                className="EditorNoteOwnerInfo__avatar"
                alt=(optionToEmptyString(user##username) ++ " avatar")
              />
          )
        )
        (
          user##username
          =>> (
            name =>
              <span className="EditorNoteOwnerInfo__username">
                name->str
              </span>
          )
        )
      </Router.Link>
      (
        noteLastEdited
        =>> (
          noteLastEdited =>
            <span className="EditorNoteOwnerInfo__lastEdited">
              "last edited"->str
              <UI_DateTime
                date=noteLastEdited
                className="EditorNoteOwnerInfo__time"
              />
            </span>
        )
      )
    </div>,
};
