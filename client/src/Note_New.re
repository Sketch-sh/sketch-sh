open Utils_GraphqlPpx;

module AddNoteGql = [%graphql
  {|
    mutation ($title: String!, $data: jsonb!, $id: String!, $userId: String!) {
      addNote: insert_note(objects: {
        title: $title,
        id: $id,
        user_id: $userId,
        data: $data
      }) {
        returning {
          id
          updated_at
          title
          data
        }
      }
    }
  |}
];

module AddNoteComponent = ReasonApollo.CreateMutation(AddNoteGql);

let blocks = [|
  {
    Editor_Types.Block.b_id: Utils.generateId(),
    b_data: Editor_Blocks_Utils.emptyCodeBlock(),
  },
|];

open Utils;
type action =
  | Saved(string, string);
let component = ReasonReact.reducerComponent("Note_New");

let make = _children : ReasonReact.component(unit, 'a, action) => {
  ...component,
  reducer: (action, _) =>
    switch (action) {
    | Saved(noteId, username) =>
      ReasonReact.SideEffects(
        (_ => Router.push(Route.Note({username, noteId, slug: None}))),
      )
    },
  render: ({send}) =>
    <Auth.IsAuthenticatedWithUserInfo>
      ...(
           fun
           | None =>
             "TODO: Handle new document when user are not login in" |. str
           | Some((user, userId)) =>
             <AddNoteComponent>
               ...(
                    (mutation, createNoteResult) => {
                      let {AddNoteComponent.loading} = createNoteResult;
                      <div>
                        <Editor_Note
                          blocks
                          loading
                          onSave=(
                            (~title, ~data) => {
                              let noteId = Utils.generateId();
                              let newNote =
                                AddNoteGql.make(
                                  ~title,
                                  ~data=data |. Editor_Types.JsonEncode.encode,
                                  ~id=noteId,
                                  ~userId,
                                  (),
                                );
                              Js.Promise.(
                                mutation(~variables=newNote##variables, ())
                                |> then_(result => {
                                     let a = result##data;

                                     Js.log(a);
                                     send(
                                       Saved(
                                         noteId,
                                         user##username
                                         |> Utils.optionToEmptyString,
                                       ),
                                     );
                                     resolve();
                                   })
                              )
                              |> ignore;
                            }
                          )
                        />
                      </div>;
                    }
                  )
             </AddNoteComponent>
         )
    </Auth.IsAuthenticatedWithUserInfo>,
};
