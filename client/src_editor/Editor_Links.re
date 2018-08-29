Modules.require("./Editor_Links.css");

module GetLink = [%graphql
  {|
    query getNote (
      $noteId: String!
    ) {
      note: note (where: {id : {_eq: $noteId}}) {
        ...GqlFragment.Editor.EditorNote
      }
    }
  |}
];

module GetLinkComponent = ReasonApollo.CreateQuery(GetLink);

open Utils;
open Editor_Types;

type uiLink = (string, string);

type link =
  | ReadyLink(Link.link)
  | UiLink(uiLink);

type linkStatus =
  | NotAsked
  | Error
  | Loading;

type action =
  | Link_Add(uiLink)
  | Link_Fetched
  | Link_Update(Link.link)
  | Link_Delete;

type state = {
  links: array(Link.link),
  fetchingLink: option((string, string)),
};

let singleLink = (name, id) =>
  <div className="link__container">
    <input className="link__input" defaultValue=id placeholder="id" />
    <input className="link__input" defaultValue=name placeholder="name" />
    <button className="link__button link__button--danger">
      <Fi.Trash2 />
    </button>
  </div>;

module EmptyLink = {
  type state = {
    name: string,
    id: string,
  };

  type action =
    | UpdateId(string)
    | UpdateName(string);

  let component = ReasonReact.reducerComponent("Editor_EmptyLink");
  let make = (~status, ~onSubmit, ~name="", ~id="", _children) => {
    ...component,
    initialState: () => {name, id},
    reducer: (action, state) =>
      switch (action) {
      | UpdateId(id) => ReasonReact.Update({...state, id})
      | UpdateName(name) => ReasonReact.Update({...state, name})
      },
    render: ({send, state}) =>
      <div>
        <input
          className="link__input"
          value=state.id
          placeholder="id"
          onChange=(id => send(UpdateId(valueFromEvent(id))))
        />
        <input
          className="link__input"
          value=state.name
          placeholder="name"
          onChange=(name => send(UpdateName(valueFromEvent(name))))
        />
        <button
          className="link__button"
          onClick=(_ => onSubmit((state.name, state.id)))
          disabled=(status == Loading)>
          <Fi.Plus />
        </button>
      </div>,
  };
};

let component = ReasonReact.reducerComponent("Editor_Links");

let make = (~links, ~onLinkAdd, _children) => {
  ...component,
  initialState: () => {links, fetchingLink: None},
  reducer: (action: action, state: state) =>
    switch (action) {
    | Link_Add((name, id)) =>
      ReasonReact.Update({...state, fetchingLink: Some((name, id))})
    | Link_Fetched => ReasonReact.Update({...state, fetchingLink: None})
    | _ => ReasonReact.NoUpdate
    },
  render: ({send, state}) =>
    <div className="links__container">
      <span className="links__disclaimer">
        "Add a link to another sketch by pasting it's id and giving it a module name"
        ->str
      </span>
      <div
        /* state.links->Belt.Array.mapU(
           (. link) =>
             switch (link) {
             | Internal({sketch_id, name}) => singleLink(name, sketch_id)
             | External () => ReasonReact.null
             })
           ->ReasonReact.array */
      />
      (
        switch (state.fetchingLink) {
        | None =>
          <EmptyLink
            status=NotAsked
            onSubmit=(uiLink => send(Link_Add(uiLink)))
          />
        | Some((name, id)) =>
          let getLinkQuery = GetLink.make(~noteId=id, ());
          <GetLinkComponent variables=getLinkQuery##variables>
            ...(
                 ({result}) =>
                   switch (result) {
                   | Loading =>
                     <EmptyLink
                       status=Loading
                       id
                       name
                       onSubmit=(uiLink => send(Link_Add(uiLink)))
                     />
                   | Error(_error) =>
                     <EmptyLink
                       status=Error
                       id
                       name
                       onSubmit=(uiLink => send(Link_Add(uiLink)))
                     />
                   | Data(response) =>
                     let notes = response##note;
                     let note = Belt.Array.getUnsafe(notes, 0);
                     Js.log(note##id);
                     Js.log(note##title);
                     onLinkAdd(note);
                     send(Link_Fetched) |> ignore;
                     <EmptyLink
                       status=Loading
                       id
                       name
                       onSubmit=(uiLink => send(Link_Add(uiLink)))
                     />;
                   }
               )
          </GetLinkComponent>;
        }
      )
    </div>,
};
