[%%debugger.chrome];
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
open Editor_Types.Link;

type name = string;
type id = string;

type uiLink = {
  name: string,
  id: string,
  timestamp: string,
};

type linkStatus =
  | NotAsked
  | Error(string)
  | Loading
  | Fetched;

type action =
  | Link_Add((name, id))
  | Link_Retry((name, id))
  | Link_Fetched(link)
  | Link_Update(link)
  | Link_Delete(link);

type state = {
  /* links: array(Link.link), */
  fetchingLink: option(uiLink),
};

module SingleLink = {
  let component = ReasonReact.statelessComponent("Editor_SingleLink");

  let make = (~name, ~id, ~timestamp, ~onDelete, _children) => {
    ...component,
    render: _ =>
      <div className="link__container">
        <input className="link__input" defaultValue=id placeholder="id" />
        <input className="link__input" defaultValue=name placeholder="name" />
        <button
          className="link__button link__button--danger"
          onClick=(_ => onDelete())>
          <Fi.Trash2 />
        </button>
        <i className="link__timestamp">
          <span className="link__timestamp__hint">
            "revision from "->str
          </span>
          <UI_DateTime date=timestamp />
        </i>
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
      | Some(f) =>
        Log.green(~label="Empty Link", "calll onFetched");
        f();
      },
    reducer: (action, state) =>
      switch (action) {
      | UpdateId(id) => ReasonReact.Update({...state, id})
      | UpdateName(name) => ReasonReact.Update({...state, name})
      },
    render: ({send, state}) =>
      <div className="link__container">
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
            | NotAsked => <Fi.PlusCircle />
            | Loading => <Fi.Loader />
            | Error(msg) => (msg ++ " Retry?")->str
            | Fetched => <Fi.Trash2 />
            }
          )
        </button>
      </div>,
  };
};

let getIdFromLink = link =>
  switch (link) {
  | Internal(link) => link.sketch_id
  | _ => failwith("There are no external links yet.")
  };

let component = ReasonReact.reducerComponent("Editor_Links");

let make = (~links, ~onUpdate, _children) => {
  ...component,
  initialState: () => {fetchingLink: None},
  reducer: (action, _state) => {
    Log.blue(~label="Editor_Links", action);
    switch (action) {
    | Link_Retry((name, id))
    | Link_Add((name, id)) =>
      let timestamp = Js.Date.make() |> Js.Date.toISOString;
      /* TODO fail if there's already a module with same name */
      /* TODO fail if id is the same as current link */
      ReasonReact.Update({fetchingLink: Some({name, id, timestamp})});
    | Link_Fetched(link) =>
      ReasonReact.UpdateWithSideEffects(
        {fetchingLink: None},
        (
          _ => {
            let id = getIdFromLink(link);
            Notify.info("Link " ++ id ++ " has been fetched.");
            let links = Belt.Array.concat(links, [|link|]);

            onUpdate(links);
          }
        ),
      )
    | Link_Delete(linkToDelete) =>
      ReasonReact.SideEffects(
        (
          _ => {
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
    };
  },
  render: ({send, state}) => {
    let existingLinks =
      links
      ->Belt.Array.mapU(
          (. link: Link.link) =>
            switch (link) {
            | Internal({sketch_id, name, revision_at}) =>
              <SingleLink
                id=sketch_id
                key=sketch_id
                name
                timestamp=revision_at
                onDelete=(() => send(Link_Delete(link)))
              />
            | _ => failwith("There are no external links yet.")
            },
        );

    <div className="links__container">
      <span className="links__disclaimer">
        "Add a link to another sketch by pasting its id and giving it a module name"
        ->str
      </span>
      <div> existingLinks->ReasonReact.array </div>
      (
        switch (state.fetchingLink) {
        | None =>
          <EmptyLink
            key="notAsked"
            status=NotAsked
            onSubmit=(uiLink => send(Link_Add(uiLink)))
          />
        | Some({name, id, timestamp}) =>
          let getLinkQuery = GetLink.make(~noteId=id, ());
          <GetLinkComponent variables=getLinkQuery##variables>
            ...(
                 ({result}) =>
                   switch (result) {
                   | Loading =>
                     <EmptyLink
                       key=timestamp
                       status=Loading
                       id
                       name
                       onSubmit=(uiLink => send(Link_Add(uiLink)))
                     />
                   | Error(_error) =>
                     <EmptyLink
                       key=timestamp
                       status=(Error("Fetching failed."))
                       id
                       name
                       onSubmit=(uiLink => send(Link_Retry(uiLink)))
                     />
                   | Data(response) =>
                     let note = response##note->Belt.Array.get(0);

                     switch (note) {
                     | Some(note) =>
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
                         key=(
                           note##updated_at
                           |> Js.Json.decodeString
                           |> Belt.Option.getExn
                         )
                         status=Fetched
                         id
                         name
                         onSubmit=(uiLink => send(Link_Add(uiLink)))
                         onFetched=(() => send(Link_Fetched(link)))
                       />;
                     | None =>
                       <EmptyLink
                         key=timestamp
                         status=(Error("No such link found."))
                         id
                         name
                         onSubmit=(uiLink => send(Link_Retry(uiLink)))
                       />
                     };
                   }
               )
          </GetLinkComponent>;
        }
      )
    </div>;
  },
};
