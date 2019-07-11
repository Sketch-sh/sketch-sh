[%%debugger.chrome];
Modules.require("./Editor_Links.css");
Modules.require("@reach/dialog/styles.css");

module Style = {
  let row = ReactDOMRe.Style.make(~justifyContent="space-between", ());
  let table = ReactDOMRe.Style.make(~width="100%", ());
};

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
  | ToggleLinkModal
  | Link_Add((name, id))
  | Link_Fetched(link)
  | Link_Refresh(link)
  | Link_Delete(link);

type state = {
  fetchingLink: option(uiLink),
  isLinkModalOpen: bool,
};

let createInternalLink = (~name, ~note) => {
  /* TODO handle links that also have links */
  let (lang, _links, blocks) =
    switch (note##data) {
    | None => (Editor_Types.RE, [||], [||])
    | Some(data) => Editor_Json.V1.decode(data)
    };

  Internal({
    revision_at: note##updated_at,
    sketch_id: note##id,
    name,
    lang,
    code: Editor_Blocks_Utils.concatCodeBlocksToString(blocks),
  });
};

module SingleLink = {
  type action =
    | Link_Refresh;

  type state = {isLinkRefreshing: bool};

  let reducer = (action, state) =>
    switch (action) {
    | Link_Refresh =>
      ReasonReact.Update({isLinkRefreshing: !state.isLinkRefreshing})
    };
  let initialState = () => {isLinkRefreshing: false};
  let component = ReasonReact.reducerComponent("Editor_Links_SingleLink");

  [@react.component]
  let make = (~name, ~id, ~timestamp=?, ~onDelete=?, ~onRefresh=?, ()) =>
    ReactCompat.useRecordApi({
      ...component,
      initialState,
      reducer,
      render: ({state, send}) =>
        <tr style=Style.row className="link__container">
          <td>
            <Router.Link
              className="link__input"
              route={Route.Note({noteId: id, data: None})}>
              id->str
            </Router.Link>
          </td>
          <td> <span className="link__input"> name->str </span> </td>
          <td>
            <span className="link__input">
              {name->String.capitalize->str}
            </span>
          </td>
          {switch (onDelete, onRefresh) {
           | (None, None) => ReasonReact.null
           | _ =>
             <td className="link__container">
               {switch (onDelete) {
                | None => ReasonReact.null
                | Some(onDelete) =>
                  <button
                    className="link__button link__button--danger action__button"
                    onClick={_ => onDelete()}>
                    <Fi.Trash2 />
                  </button>
                }}
               {switch (onRefresh) {
                | None => ReasonReact.null
                | Some(onRefresh) =>
                  <>
                    {!state.isLinkRefreshing
                       ? <button
                           className="link__button link__button--danger action__button"
                           onClick={_ => send(Link_Refresh)}>
                           <Fi.FiRefreshCw />
                         </button>
                       : <Fi.Loader />}
                    {!state.isLinkRefreshing
                       ? ReasonReact.null
                       : {
                         let getLinkQuery = GetLink.make(~noteId=id, ());
                         <GetLinkComponent
                           variables={
                             getLinkQuery##variables;
                           }>
                           ...{({result}) => {
                             switch (result) {
                             | Loading => ()

                             | Error(_error) =>
                               Notify.error(
                                 "Fetching link " ++ id ++ " failed.",
                               );
                               send(Link_Refresh);

                             | Data(response) =>
                               let note = response##note->Belt.Array.get(0);
                               switch (note) {
                               | Some(note) =>
                                 let link: Link.link =
                                   createInternalLink(~name, ~note);
                                 onRefresh(link);
                                 send(Link_Refresh);
                               | None =>
                                 Notify.error("Link " ++ id ++ " not found.")
                               };
                             };
                             ReasonReact.null;
                           }}
                         </GetLinkComponent>;
                       }}
                  </>
                }}
             </td>
           }}
          <td>
            {switch (timestamp) {
             | None => ReasonReact.null
             | Some(timestamp) =>
               <span className="link__timestamp">
                 <span className="link__timestamp__hint">
                   "Revision from "->str
                 </span>
                 <UI_DateTime date=timestamp />
               </span>
             }}
          </td>
        </tr>,
    });
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

  [@react.component]
  let make = (~status, ~onSubmit, ~onFetched=?, ~name="", ~id="", ()) =>
    ReactCompat.useRecordApi({
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
        | UpdateName(name) =>
          ReasonReact.Update({...state, name, dirty: true})
        },
      render: ({send, state}) =>
        <>
          <hr />
          <span className="links__new"> "New Link"->str </span>
          <table className="links__container">
            <tbody>
              <tr>
                <td>
                  <input
                    className="link__input"
                    value={state.id}
                    placeholder="id"
                    onChange={id => send(UpdateId(valueFromEvent(id)))}
                  />
                </td>
                <td>
                  <input
                    className="link__input"
                    value={state.name}
                    placeholder="name"
                    onChange={name =>
                      send(UpdateName(valueFromEvent(name)))
                    }
                  />
                </td>
                <td>
                  <button
                    className="link__button"
                    onClick={_ =>
                      if (state.name != "" && state.id != "") {
                        onSubmit((state.name, state.id));
                      }
                    }
                    disabled={status == Loading}>
                    {switch (status, state.dirty) {
                     | (NotAsked, _) => <Fi.PlusCircle />
                     | (Loading, _) => <Fi.Loader />
                     | (Error(message), false) =>
                       <>
                         <Fi.PlusCircle />
                         <i className="link__button__error">
                           {("  " ++ message)->str}
                         </i>
                       </>
                     | (Error(_), true) => <Fi.PlusCircle />
                     | (Fetched, _) => <Fi.Trash2 />
                     }}
                  </button>
                </td>
              </tr>
            </tbody>
          </table>
        </>,
    });
};

let getEmptyLink =
    (~id, ~name, ~timestamp, ~status, ~onSubmit, ~onFetched=() => (), ()) =>
  <EmptyLink key=timestamp status id name onSubmit onFetched />;

let component = ReasonReact.reducerComponent("Editor_Links");

[@react.component]
let make = (~currentSketchId, ~links, ~onUpdate, ()) =>
  ReactCompat.useRecordApi({
    ...component,
    initialState: () => {fetchingLink: None, isLinkModalOpen: false},
    reducer: (action, state) =>
      switch (action) {
      | ToggleLinkModal =>
        ReasonReact.Update({
          ...state,
          isLinkModalOpen: !state.isLinkModalOpen,
        })

      | Link_Add((name, id)) =>
        let timestamp = Js.Date.make() |> Js.Date.toISOString;

        if (id == currentSketchId) {
          ReasonReact.SideEffects(
            _ => Notify.error("Cannot link current sketch."),
          );
        } else {
          let linkWithSameNameExists =
            arrayFind(links, l => getNameFromLink(l) == name);

          switch (linkWithSameNameExists) {
          | Some(_) =>
            ReasonReact.SideEffects(
              _ => Notify.error("Link with same module name already exists."),
            )
          | None =>
            let linkWithSameIdExists =
              arrayFind(links, l => getIdFromLink(l) == id);

            switch (linkWithSameIdExists) {
            | Some(_) =>
              ReasonReact.SideEffects(
                _ => Notify.error("Link with same id already exists."),
              )
            | None =>
              ReasonReact.Update({
                ...state,
                fetchingLink: Some({name, id, timestamp}),
              })
            };
          };
        };
      | Link_Fetched(link) =>
        ReasonReact.UpdateWithSideEffects(
          {...state, fetchingLink: None},
          _ => {
            let id = getIdFromLink(link);
            Notify.info("Link " ++ id ++ " has been fetched.");

            let links = Belt.Array.concat(links, [|link|]);
            onUpdate(links);
          },
        )
      | Link_Delete(linkToDelete) =>
        ReasonReact.SideEffects(
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
          },
        )
      | Link_Refresh(linkToRefresh) =>
        ReasonReact.SideEffects(
          _ =>
            onUpdate(
              Belt.Array.mapU(links, (. link) =>
                switch (link, linkToRefresh) {
                | (Internal(linkA), Internal(linkB)) =>
                  linkA.sketch_id == linkB.sketch_id ? linkToRefresh : link
                | _ => link
                }
              ),
            ),
        )
      },
    render: ({send, state}) => {
      let onSubmit = uiLink => send(Link_Add(uiLink));
      let displayLinks =
        links->Belt.Array.mapU((. link: Link.link) =>
          switch (link) {
          | Internal({sketch_id, name, revision_at}) =>
            <SingleLink
              id=sketch_id
              key=sketch_id
              name
              timestamp=revision_at
            />
          | _ => failwith("There are no external links yet.")
          }
        );
      let modalLinks =
        links->Belt.Array.mapU((. link: Link.link) =>
          switch (link) {
          | Internal({sketch_id, name, revision_at}) =>
            <SingleLink
              id=sketch_id
              key=sketch_id
              name
              timestamp=revision_at
              onDelete={() => send(Link_Delete(link))}
              onRefresh={link => send(Link_Refresh(link))}
            />
          | _ => failwith("There are no external links yet.")
          }
        );
      <>
        <br />
        <span id="linkedLists" className="UI_SketchOwnerInfo__username">
          {str("Linked Links")}
        </span>
        <UI_Balloon position=Down message="Edit sketches">
          ...<button
               className="EditorNote__linkMenu"
               onClick={_ => send(ToggleLinkModal)}>
               <Fi.Edit2 />
             </button>
        </UI_Balloon>
        <hr />
        <table style=Style.table>
          <tbody> displayLinks->ReasonReact.array </tbody>
        </table>
        <hr />
        <Reach_Modal.DialogOverlay
          isOpen={state.isLinkModalOpen}
          className="EditorNote__Modal--overlay"
          onDismiss={() => send(ToggleLinkModal)}>
          <Reach_Modal.DialogContent className="EditorNote__Modal--content">
            <button
              className="EditorNote__linkMenu EditorNote__Modal--content--close"
              onClick={_ => send(ToggleLinkModal)}>
              <Fi.FiXCircle />
            </button>
            <h2 className="links__new"> {str("Linkings")} </h2>
            <div className="links__container">
              <span className="links__disclaimer">
                "Add a link to another sketch by pasting its id and assigning it a module name."
                ->str
              </span>
              <table style=Style.table>
                <tbody> modalLinks->ReasonReact.array </tbody>
              </table>
              {switch (state.fetchingLink) {
               | None => <EmptyLink key="notAsked" status=NotAsked onSubmit />
               | Some({name, id, timestamp}) =>
                 let getLinkQuery = GetLink.make(~noteId=id, ());
                 <GetLinkComponent variables=getLinkQuery##variables>
                   ...{({result}) => {
                     let emptyLink =
                       getEmptyLink(~timestamp, ~id, ~name, ~onSubmit);
                     switch (result) {
                     | Loading => emptyLink(~status=Loading, ())

                     | Error(_error) =>
                       Notify.error("Fetching link " ++ id ++ " failed.");
                       emptyLink(~status=Error({|Fetching failed.|}), ());

                     | Data(response) =>
                       let note = response##note->Belt.Array.get(0);

                       switch (note) {
                       | Some(note) =>
                         let link: Link.link =
                           createInternalLink(~name, ~note);
                         let timestamp =
                           note##updated_at
                           |> Js.Json.decodeString
                           |> Belt.Option.getExn;
                         let onFetched = () => send(Link_Fetched(link));
                         getEmptyLink(
                           ~status=Fetched,
                           ~id,
                           ~name,
                           ~onSubmit,
                           ~timestamp,
                           ~onFetched,
                           (),
                         );

                       | None =>
                         Notify.error("Link " ++ id ++ " not found.");
                         emptyLink(
                           ~status=Error({|No such link found.|}),
                           (),
                         );
                       };
                     };
                   }}
                 </GetLinkComponent>;
               }}
            </div>
          </Reach_Modal.DialogContent>
        </Reach_Modal.DialogOverlay>
      </>;
    },
  });
