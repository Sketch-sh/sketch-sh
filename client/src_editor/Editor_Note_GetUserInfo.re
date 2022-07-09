open Utils;
open GqlUserInfo;

[@react.component]
let make = (~userId, ~children) => {
  let query = UserInfoGql.make(~userId, ());

  <UserInfoComponent variables=query##variables>
    {({result}) =>
       switch (result) {
       | Loading => children(None)
       | Error(_) => children(None)
       | Data(response) =>
         response##user
         ->(
             arrayFirst(~empty=children(None), ~render=user =>
               children(Some(user))
             )
           )
       }}
  </UserInfoComponent>;
};
