open Utils_GraphqlPpx;

module GetNote = [%graphql
  {|
    query getNote (
      $noteId: String!
    ) {
      note (
        where: {
          id : {_eq: $noteId}
        }
      ) {
        id
        title
        data @bsDecoder(fn: "decodeBlockData")
        updated_at
        owner {
          id
          username
          avatar
        }
      }
    }
  |}
];

module GetNoteComponent = ReasonApollo.CreateQuery(GetNote);

module UpdateNoteGql = [%graphql
  {|
    mutation ($noteId: String!, $data: jsonb!, $title: String!) {
      update_note(
        where: {
          id: {_eq: $noteId}
        }
        _set: {
        title: $title,
        data: $data
      }) {
        returning {
          updated_at
        }
      }
    }
  |}
];

module UpdateNoteComponent = ReasonApollo.CreateMutation(UpdateNoteGql);

open Utils;

let component = ReasonReact.statelessComponent("Note");

let make = (~noteInfo: Route.noteRouteConfig, _children) => {
  ...component,
  render: _self => {
    let noteQuery = GetNote.make(~noteId=noteInfo.noteId, ());
    <GetNoteComponent variables=noteQuery##variables>
      ...(
           ({result}) =>
             switch (result) {
             | Loading => <div> (ReasonReact.string("Loading")) </div>
             | Error(error) =>
               <div> (ReasonReact.string(error##message)) </div>
             | Data(response) =>
               let notes = response##note;
               notes
               |. arrayFirst(~empty=<NotFound entity="note" />, ~render=note =>
                    <UpdateNoteComponent>
                      ...(
                           (mutation, createNoteResult) => {
                             let {UpdateNoteComponent.loading} = createNoteResult;
                             <div>
                               <Editor_Note
                                 blocks=(
                                   switch (note##data) {
                                   | None => [||]
                                   | Some(blocks) => blocks
                                   }
                                 )
                                 title=(note##title |. optionToEmptyString)
                                 loading
                                 onSave=(
                                   (~title, ~data) => {
                                     let updatedNote =
                                       UpdateNoteGql.make(
                                         ~title,
                                         ~data=
                                           data
                                           |. Editor_Types.JsonEncode.encode,
                                         ~noteId=noteInfo.noteId,
                                         (),
                                       );
                                     mutation(
                                       ~variables=updatedNote##variables,
                                       ~refetchQueries=[|"getNote"|],
                                       (),
                                     )
                                     |> ignore;
                                   }
                                 )
                               />
                             </div>;
                           }
                         )
                    </UpdateNoteComponent>
                  );
             }
         )
    </GetNoteComponent>;
  },
};
