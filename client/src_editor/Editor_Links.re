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
open Editor_Links_Utils;

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
  | Link_Fetched(link)
  | Link_Update(link)
  | Link_Delete(link);

type state = {fetchingLink: option(uiLink)};

module SingleLink = {
  let component = ReasonReact.statelessComponent("Editor_Links_SingleLink");

  let make = (~name, ~id, ~timestamp, ~onDelete, _children) => {
    ...component,
    render: _ =>
      <div className="link__container">
        <input
          className="link__input"
          defaultValue=id
          placeholder="id"
          disabled=true
        />
        <input
          className="link__input"
          defaultValue=name
          placeholder="name"
          disabled=true
        />
        <button
          className="link__button link__button--danger"
          onClick=(_ => onDelete())>
          <Fi.Trash2 />
        </button>
        <i className="link__timestamp">
          <span className="link__timestamp__hint">
            "Revision from "->str
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
    dirty: bool,
  };

  type action =
    | UpdateId(string)
    | UpdateName(string);

  let component = ReasonReact.reducerComponent("Editor_Links_EmptyLink");

  let make = (~status, ~onSubmit, ~onFetched=?, ~name="", ~id="", _children) => {
    ...component,
    initialState: () => {name, id, dirty: false},
    didMount: _ =>
      switch (onFetched) {
      | None => ()
      | Some(f) => f()
      },
    reducer: (action, state) =>
      switch (action) {
      | UpdateId(id) => ReasonReact.Update({...state, id, dirty: true})
      | UpdateName(name) => ReasonReact.Update({...state, name, dirty: true})
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
          onClick=(
            _ =>
              if (state.name != "" && state.id != "") {
                onSubmit((state.name, state.id));
              }
          )
          disabled=(status == Loading)>
          (
            switch (status, state.dirty) {
            | (NotAsked, _) => <Fi.PlusCircle />
            | (Loading, _) => <Fi.Loader />
            | (Error(message), false) =>
              <>
                <Fi.PlusCircle />
                <i className="link__button__error"> ("  " ++ message)->str </i>
              </>
            | (Error(_), true) => <Fi.PlusCircle />
            | (Fetched, _) => <Fi.Trash2 />
            }
          )
        </button>
      </div>,
  };
};

let component = ReasonReact.reducerComponent("Editor_Links");

let make = (~currentSketchId, ~links, ~onUpdate, _children) => {
  ...component,
  initialState: () => {fetchingLink: None},
  reducer: (action, _state) =>
    switch (action) {
    | Link_Add((name, id)) =>
      let timestamp = Js.Date.make() |> Js.Date.toISOString;

      if (id == currentSketchId) {
        ReasonReact.SideEffects(
          (_ => Notify.error("Cannot link current sketch.")),
        );
      } else {
        let linkWithSameNameExists =
          arrayFind(links, l => getNameFromLink(l) == name);

        switch (linkWithSameNameExists) {
        | Some(_) =>
          ReasonReact.SideEffects(
            (_ => Notify.error("Link with same module name already exists.")),
          )
        | None =>
          let linkWithSameIdExists =
            arrayFind(links, l => getIdFromLink(l) == id);

          switch (linkWithSameIdExists) {
          | Some(_) =>
            ReasonReact.SideEffects(
              (_ => Notify.error("Link with same id already exists.")),
            )
          | None =>
            ReasonReact.Update({fetchingLink: Some({name, id, timestamp})})
          };
        };
      };
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
        "Add a link to another sketch by pasting its id and assigning it a module name."
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
                     Notify.error("Fetching link " ++ id ++ " failed.");

                     <EmptyLink
                       key=timestamp
                       status=(Error("Fetching failed."))
                       id
                       name
                       onSubmit=(uiLink => send(Link_Add(uiLink)))
                     />;
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
                       Notify.error("Link " ++ id ++ " not found.");

                       <EmptyLink
                         key=timestamp
                         status=(Error("No such link found."))
                         id
                         name
                         onSubmit=(uiLink => send(Link_Add(uiLink)))
                       />;
                     };
                   }
               )
          </GetLinkComponent>;
        }
      )
    </div>;
  },
};
