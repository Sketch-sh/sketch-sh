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

type linkStatus =
  | NotAsked
  | Error
  | Loading
  | Fetched;

type action =
  | Link_Add(uiLink)
  | Link_Fetched(Link.link)
  | Link_Update(Link.link)
  | Link_Delete(Link.link);

type state = {
  links: array(Link.link),
  fetchingLink: option((string, string)),
};

module SingleLink = {
  let component = ReasonReact.statelessComponent("Editor_SingleLink");

  let make = (~name, ~id, ~onDelete, _children) => {
    ...component,
    render: _ =>
      <div>
        <input className="link__input" defaultValue=id placeholder="id" />
        <input className="link__input" defaultValue=name placeholder="name" />
        <button
          className="link__button link__button--danger"
          onClick=(_ => onDelete())>
          <Fi.Trash2 />
        </button>
      </div>,
  };
};

module EmptyLink = {
  type state = {
    name: string,
    id: string,
  };

  type action =
    | UpdateId(string)
    | UpdateName(string);

  let component = ReasonReact.reducerComponent("Editor_EmptyLink");
  let make = (~status, ~onSubmit, ~onFetched=?, ~name="", ~id="", _children) => {
    ...component,
    initialState: () => {name, id},
    didMount: _ =>
      switch (onFetched) {
      | None => ()
      | Some(f) => f()
      },
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
          (
            switch (status) {
            | NotAsked => <Fi.Plus />
            | Loading => <Fi.Loader />
            | Error => "TODO"->str
            | Fetched => <Fi.Trash2 />
            }
          )
        </button>
      </div>,
  };
};

let component = ReasonReact.reducerComponent("Editor_Links");

let make = (~links, ~onUpdate, _children) => {
  ...component,
  initialState: () => {links, fetchingLink: None},
  reducer: (action: action, state: state) =>
    switch (action) {
    | Link_Add((name, id)) =>
      ReasonReact.Update({...state, fetchingLink: Some((name, id))})
    | Link_Fetched(link) =>
      ReasonReact.UpdateWithSideEffects(
        {...state, fetchingLink: None},
        (
          _ => {
            let links = Belt.Array.concat(links, [|link|]);

            onUpdate(links);
          }
        ),
      )
    | Link_Delete(linkToDelete) =>
      ReasonReact.SideEffects(
        (
          _ => {
            open Editor_Types.Link;
            let links =
              Belt.Array.keepU(links, (. link) =>
                switch (link, linkToDelete) {
                | (Internal(linkA), Internal(linkB)) =>
                  linkA.sketch_id != linkB.sketch_id
                | _ => true
                }
              );

            onUpdate(links);
          }
        ),
      )
    | _ => ReasonReact.NoUpdate
    },
  render: ({send, state}) => {
    let existingLinks =
      links
      ->Belt.Array.mapU(
          (. link: Link.link) =>
            switch (link) {
            | Internal({sketch_id, name}) =>
              <SingleLink
                id=sketch_id
                key=sketch_id
                name
                onDelete=(() => send(Link_Delete(link)))
              />
            | External () => ReasonReact.null
            },
        );

    <div className="links__container">
      <span className="links__disclaimer">
        "Add a link to another sketch by pasting it's id and giving it a module name"
        ->str
      </span>
      <div> existingLinks->ReasonReact.array </div>
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
                     let note = response##note[0];

                     /* TODO handle links that also have links */
                     let (lang, _links, blocks) =
                       switch (note##data) {
                       | None => (Editor_Types.RE, [||], [||])
                       | Some(data) => Editor_Json.V1.decode(data)
                       };

                     let link: Link.link =
                       Internal({
                         revision_at: note##updated_at,
                         sketch_id: note##id,
                         name,
                         lang,
                         code:
                           Editor_Blocks_Utils.concatCodeBlocksToString(
                             blocks,
                           ),
                       });

                     <EmptyLink
                       status=Fetched
                       id
                       name
                       onSubmit=(uiLink => send(Link_Add(uiLink)))
                       onFetched=(() => send(Link_Fetched(link)))
                     />;
                   }
               )
          </GetLinkComponent>;
        }
      )
    </div>;
  },
};
