[@bs.config {jsx: 3}];
open Utils;

Modules.require("./UI_SketchOwnerInfo.css");

[@react.component]
let make = (~owner, ~noteLastEdited=?, ~className=?) => {
  <div ?className>
    <Router.Link route={Route.User(owner##username)}>
      {owner##avatar
       =>> (
         avatar =>
           <img
             src=avatar
             width="24"
             height="24"
             className="UI_SketchOwnerInfo__avatar"
             alt={owner##username ++ " avatar"}
           />
       )}
      <span className="UI_SketchOwnerInfo__username">
        {owner##username->str}
      </span>
    </Router.Link>
    {noteLastEdited
     =>> (
       noteLastEdited =>
         <span className="UI_SketchOwnerInfo__lastEdited">
           "last edited"->str
           <UI_DateTime
             date=noteLastEdited
             className="UI_SketchOwnerInfo__time"
           />
         </span>
     )}
  </div>;
};

module Jsx2 = {
  let component = ReasonReact.statelessComponent(__MODULE__);
  let make = (~owner, ~noteLastEdited=?, ~className=?) => {
    ReasonReactCompat.wrapReactForReasonReact(
      make,
      makeProps(~owner, ~noteLastEdited?, ~className?, ()),
    );
  };
};
