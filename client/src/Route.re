/*
   Route.re
   This module contains type definition for all routes in the project
 */
type noteRouteConfig = {
  username: string,
  noteId: string,
  slug: option(string),
};

type t =
  | Home
  | Note(noteRouteConfig)
  | NoteNew
  | AuthGithub
  | AuthFailure
  | AuthCallback(string)
  | EditorDevelopment
  | NotFound;

type route = t;

let routeToUrl: t => string =
  fun
  | Home => "/"
  | Note({username, noteId, slug}) =>
    {j|/u/$(username)/$(noteId)/|j}
    ++ (
      switch (slug) {
      | None => ""
      | Some(slug) => slug
      }
    )
  | NoteNew => "/new"
  | AuthGithub => "/auth/github"
  | AuthFailure => "/auth/failure"
  | AuthCallback(token) => "/auth/callback?token=" ++ token
  | EditorDevelopment => "/____EDITOR-DEVELOPMENT____"
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
    | ["new"] => NoteNew
    | ["____EDITOR-DEVELOPMENT____"] => EditorDevelopment
    | ["auth", "github"] => AuthGithub
    | ["auth", "failure"] => AuthFailure
    | ["auth", "callback"] =>
      /*
       NOTE: Move querystring parsing outside if there are more
       branches use it
       */
      let parsedQueryString = url.search |. URLSearchParams.make;

      switch (parsedQueryString |. URLSearchParams.get("token")) {
      | Some(token) => AuthCallback(token)
      | None => NotFound
      };
    | ["u", username, noteId] => Note({username, noteId, slug: None})
    | ["u", username, noteId, slug] =>
      Note({username, noteId, slug: Some(slug)})
    | _ => NotFound
    };
