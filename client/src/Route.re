/*
   Route.re
   This module contains type definition for all routes in the project
 */
type noteRouteConfig = {
  noteId: string,
  data: option(string),
};

type t =
  | Home
  | Note(noteRouteConfig)
  | NoteNew(Editor_Types.lang)
  | NoteTemplateChoose
  | User(string)
  | AuthGithub
  | AuthFailure
  | AuthCallback(string)
  | AuthLogout
  | NotFound;

type route = t;

let routeToUrl: t => string =
  fun
  | Home => "/"
  | Note({noteId, data: _}) => {j|/s/$(noteId)/|j}
  | NoteNew(ML) => "/new/ocaml"
  | NoteNew(RE) => "/new/reason"
  | NoteTemplateChoose => "/new"
  | User(userName) => "/u/" ++ userName
  | AuthGithub => "/auth/github"
  | AuthFailure => "/auth/failure"
  | AuthCallback(token) => "/auth/callback?token=" ++ token
  | AuthLogout => "/auth/logout"
  | NotFound =>
    raise(
      Invalid_argument("You're trying to navigate to a not found route"),
    );

let urlToRoute: ReasonReact.Router.url => t =
  url =>
    switch (url.path) {
    | [""]
    | []
    | ["/"] => Home
    | ["re"]
    | ["new", "reason"] => NoteNew(RE)
    | ["ml"]
    | ["new", "ocaml"]
    | ["new", "ml"] => NoteNew(ML)
    | ["new"] => NoteTemplateChoose
    | ["u", username] => User(username)
    | ["auth", "github"] => AuthGithub
    | ["auth", "failure"] => AuthFailure
    | ["auth", "callback"] =>
      /*
       NOTE: Move querystring parsing outside if there are more
       branches use it
       */
      let parsedQueryString = url.search->URLSearchParams.make;

      switch (parsedQueryString->(URLSearchParams.get("token"))) {
      | Some(token) => AuthCallback(token)
      | None => NotFound
      };
    | ["auth", "logout"] => AuthLogout
    | ["s", noteId] => Note({noteId, data: None})
    | ["s", noteId, data] => Note({noteId, data: Some(data)})
    | _ => NotFound
    };
