open GqlUserInfo;

open Utils;

[@react.component]
let make = (~userId) => {
  let query = UserInfoGql.make(~userId, ());
  <UserInfoComponent variables=query##variables>
    ...{({result}) =>
      switch (result) {
      | Loading => <div> {ReasonReact.string("Loading")} </div>
      | Error(error) => <div> {ReasonReact.string(error##message)} </div>
      | Data(response) =>
        response##user
        ->(
            arrayFirst(
              ~empty=("Can't find user with id" ++ userId)->str, ~render=user =>
              <dl>
                {user##name
                 =>> (
                   name => <> <dt> "Name"->str </dt> <dd> name->str </dd> </>
                 )}
                <>
                  <dt> "Username"->str </dt>
                  <dd> {user##username->str} </dd>
                </>
                {user##avatar
                 =>> (
                   avatar =>
                     <img
                       src=avatar
                       width="80"
                       height="80"
                       alt={user##username ++ " avatar"}
                     />
                 )}
              </dl>
            )
          )
      }
    }
  </UserInfoComponent>;
};
