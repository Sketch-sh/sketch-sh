[%%debugger.chrome];
Modules.require("./Editor_Blocks.css");

open Utils;
open Editor_Types;
open Editor_Types.Block;
open Editor_Blocks_Utils;

type action =
  | Block_Add(id, blockTyp)
  | Block_Execute(bool)
  | Block_FocusNextBlockOrCreate
  | Block_QueueDelete(id)
  | Block_DeleteQueued(id)
  | Block_Restore(id)
  | Block_CaptureQueuedMeta(id, Js.Global.timeoutId, blockData)
  | Block_Focus(id, blockTyp)
  | Block_Blur(id)
  | Block_UpdateValue(id, string, CodeMirror.EditorChange.t)
  | Block_AddWidgets(id, array(Widget.t))
  | Block_FocusUp(id)
  | Block_FocusDown(id)
  | Block_RefmtAsLang(lang)
  | Block_RefmtOnDemand
  | Block_CleanBlocksCopy
  | Block_MapRefmtToBlocks(string, id, bool);

type state = {
  lang,
  blocks: array(block),
  blocksCopy: option(array(block)),
  deletedBlockMeta: array(deletedBlockMeta),
  stateUpdateReason: option(action),
  focusedBlock: option((id, blockTyp, focusChangeType)),
};

let blockControlsButtons = (blockId, isDeleted, send) =>
  <div className="block__controls--buttons">
    <UI_Balloon message="Add code block" position=Down>
      ...<button
           className="block__controls--button"
           ariaLabel="Add code block"
           onClick=(_ => send(Block_Add(blockId, BTyp_Code)))>
           <Fi.Code />
           <sup> "+"->str </sup>
         </button>
    </UI_Balloon>
    <UI_Balloon message="Add text block" position=Down>
      ...<button
           className="block__controls--button"
           ariaLabel="Add text block"
           onClick=(_ => send(Block_Add(blockId, BTyp_Text)))>
           <Fi.Edit2 />
           <sup> "+"->str </sup>
         </button>
    </UI_Balloon>
    (
      !isDeleted ?
        <UI_Balloon message="Delete block" position=Down>
          ...<button
               className="block__controls--button block__controls--danger"
               ariaLabel="Delete block"
               onClick=(_ => send(Block_QueueDelete(blockId)))>
               <Fi.Trash2 />
               <sup> "-"->str </sup>
             </button>
        </UI_Balloon> :
        <UI_Balloon message="Restore block" position=Down>
          ...<button
               className="block__controls--button"
               ariaLabel="Restore block"
               onClick=(_ => send(Block_Restore(blockId)))>
               <Fi.RefreshCw />
               <sup> "+"->str </sup>
             </button>
        </UI_Balloon>
    )
  </div>;

let component = ReasonReact.reducerComponent("Editor_Page");

let make =
    (
      ~lang=RE,
      ~blocks: array(block),
      ~readOnly=false,
      ~onUpdate,
      ~registerExecuteCallback=?,
      ~registerShortcut: option(Shortcut.subscribeFun)=?,
      _children,
    ) => {
  let makeInitialState = () => {
    lang,
    blocks: blocks->syncLineNumber,
    blocksCopy: None,
    deletedBlockMeta: [||],
    stateUpdateReason: None,
    focusedBlock: None,
  };
  {
    ...component,
    initialState: makeInitialState,
    willReceiveProps: ({state}) =>
      if (state.lang != lang) {
        switch (state.blocksCopy) {
        | None => {...state, lang}
        | Some(blocksCopy) => {
            ...state,
            lang,
            blocks: blocksCopy,
            blocksCopy: None,
          }
        };
      } else if (blocks != state.blocks) {
        makeInitialState();
      } else {
        state;
      },
    didMount: self => {
      self.send(Block_Execute(false));
      switch (registerExecuteCallback) {
      | None => ()
      | Some(register) => register(() => self.send(Block_Execute(false)))
      };
      switch (registerShortcut) {
      | None => ()
      | Some(registerShortcut) =>
        let unReg =
          registerShortcut(
            ~global=true,
            "mod+enter",
            event => {
              open Webapi.Dom;

              event->KeyboardEvent.preventDefault;
              self.send(Block_Execute(false));
            },
          );
        let unReg2 =
          registerShortcut(
            ~global=true,
            "shift+enter",
            event => {
              open Webapi.Dom;

              event->KeyboardEvent.preventDefault;
              self.send(Block_Execute(true));
            },
          );
        let unReg3 =
          registerShortcut(
            ~global=true,
            "ctrl+shift+i",
            event => {
              open Webapi.Dom;
              event->KeyboardEvent.preventDefault;
              self.send(Block_RefmtOnDemand);
            },
          );
        self.onUnmount(() => {
          unReg();
          unReg2();
          unReg3();
        });
      };
    },
    didUpdate: ({oldSelf, newSelf}) => {
      if (oldSelf.state.lang != lang) {
        switch (newSelf.state.blocksCopy) {
        | None =>
          Js.log("no blocksCopy, refmt everything");
          newSelf.send(Block_RefmtAsLang(lang));
        | Some(_) => ()
        };
      };
      if (oldSelf.state.blocks !== newSelf.state.blocks) {
        switch (newSelf.state.stateUpdateReason) {
        | None => ()
        | Some(action) =>
          switch (action) {
          | Block_Focus(_, _)
          | Block_Blur(_)
          | Block_AddWidgets(_, _)
          | Block_FocusUp(_)
          | Block_FocusDown(_)
          | Block_FocusNextBlockOrCreate
          | Block_CaptureQueuedMeta(_, _, _)
          | Block_QueueDelete(_)
          | Block_RefmtAsLang(_)
          | Block_CleanBlocksCopy
          | Block_RefmtOnDemand
          | Block_MapRefmtToBlocks(_, _, _)
          | Block_Execute(_) => ()
          | Block_Add(_, _)
          | Block_DeleteQueued(_)
          | Block_Restore(_) => onUpdate(newSelf.state.blocks)
          | Block_UpdateValue(_, _, diff) =>
            switch (newSelf.state.blocksCopy) {
            | None => ()
            | Some(_) =>
              if (diff->CodeMirror.EditorChange.originGet != "setValue") {
                Js.log("first edit after lang change");
                newSelf.send(Block_CleanBlocksCopy);
              }
            };
            onUpdate(newSelf.state.blocks);
          }
        };
      };
    },
    reducer: (action, state) => {
      Js.log(action);
      switch (action) {
      | Block_CleanBlocksCopy =>
        ReasonReact.Update({
          ...state,
          blocksCopy: None,
          stateUpdateReason: Some(action),
        })
      | Block_RefmtOnDemand =>
        ReasonReact.SideEffects(
          (
            ({send}) =>
              switch (state.lang) {
              | ML => () /* pretty print currently unavailable in OCaml */
              | RE =>
                let callback = (code, blockId) =>
                  send(Block_MapRefmtToBlocks(code, blockId, false));
                state.blocks
                ->Belt.Array.forEachU(
                    (
                      (. {b_id, b_data}) =>
                        switch (b_data) {
                        | B_Text(_) => ()
                        | B_Code({bc_value}) =>
                          Editor_Blocks_Refmt.prettyPrintRe(
                            bc_value,
                            b_id,
                            callback,
                          )
                        }
                    ),
                  );
              }
          ),
        )
      | Block_RefmtAsLang(lang) =>
        ReasonReact.UpdateWithSideEffects(
          {
            ...state,
            blocksCopy: Some(state.blocks),
            stateUpdateReason: Some(action),
          },
          (
            ({state, send}) => {
              let callback = (code, blockId) =>
                send(Block_MapRefmtToBlocks(code, blockId, true));
              state.blocks
              ->Belt.Array.forEachU(
                  (. {b_id, b_data}) =>
                    switch (b_data) {
                    | B_Text(_) => ()
                    | B_Code({bc_value}) =>
                      Editor_Blocks_Refmt.refmtAsLangSibling(
                        bc_value,
                        lang,
                        b_id,
                        callback,
                      )
                      ->ignore
                    },
                );
            }
          ),
        )
      | Block_MapRefmtToBlocks(newValue, blockId, executeWhenDone) =>
        let blockIndex = state.blocks->getBlockIndex(blockId);
        ReasonReact.UpdateWithSideEffects(
          {
            ...state,
            stateUpdateReason: Some(action),
            blocks:
              state.blocks
              ->(
                  Belt.Array.mapWithIndexU((. i, block) => {
                    let {b_data} = block;
                    if (i != blockIndex) {
                      block;
                    } else {
                      switch (b_data) {
                      | B_Code(bcode) => {
                          ...block,
                          b_data:
                            B_Code({
                              ...bcode,
                              bc_value: newValue,
                              bc_widgets: [||],
                            }),
                        }
                      | B_Text(_) => block
                      };
                    };
                  })
                ),
          },
          (
            ({send}) =>
              executeWhenDone ?
                switch (state.blocks->findLastCodeBlock) {
                | Some(b_id) =>
                  b_id == blockId ? send(Block_Execute(false)) : ()
                | None => ()
                } :
                ()
          ),
        );
      | Block_AddWidgets(blockId, widgets) =>
        ReasonReact.Update({
          ...state,
          stateUpdateReason: Some(action),
          blocks:
            state.blocks
            ->(
                Belt.Array.mapU((. block) => {
                  let {b_id, b_data} = block;
                  if (b_id != blockId) {
                    block;
                  } else {
                    switch (b_data) {
                    | B_Text(_) => block
                    | B_Code(bcode) => {
                        ...block,
                        b_data: B_Code({...bcode, bc_widgets: widgets}),
                      }
                    };
                  };
                })
              ),
        })
      | Block_FocusNextBlockOrCreate =>
        let blockLength = state.blocks->Belt.Array.length;
        let nextBlockIndex =
          switch (state.focusedBlock) {
          | None => blockLength - 1
          | Some((id, _blockTyp, _)) =>
            switch (state.blocks->arrayFindIndex((({b_id}) => b_id == id))) {
            | None => blockLength - 1
            | Some(index) => index
            }
          };
        let findBlockId = index => {
          let {b_id, b_data} = state.blocks[index];
          (b_id, blockDataToBlockTyp(b_data));
        };
        if (nextBlockIndex == blockLength - 1) {
          ReasonReact.SideEffects(
            (
              ({send}) =>
                send(Block_Add(findBlockId(nextBlockIndex)->fst, BTyp_Code))
            ),
          );
        } else if (nextBlockIndex < blockLength - 1) {
          let (blockId, blockTyp) = findBlockId(nextBlockIndex + 1);
          ReasonReact.Update({
            ...state,
            stateUpdateReason: Some(action),
            focusedBlock:
              Some((blockId, blockTyp, FcTyp_BlockExecuteAndFocusNextBlock)),
          });
        } else {
          ReasonReact.NoUpdate;
        };
      | Block_Execute(focusNextBlock) =>
        let allCodeToExecute =
          state.blocks
          ->(
              Belt.Array.reduceU([], (. acc, {b_id, b_data, b_deleted}) =>
                b_deleted ?
                  acc :
                  (
                    switch (b_data) {
                    | B_Text(_) => acc
                    | B_Code({bc_value}) => [(b_id, bc_value), ...acc]
                    }
                  )
              )
            )
          ->Belt.List.reverse;

        /* Clear all widgets and execute all blocks */
        ReasonReact.SideEffects(
          (
            self =>
              Js.Promise.(
                Editor_Worker.executeMany(. lang, allCodeToExecute)
                |> then_(results => {
                     results
                     ->(
                         Belt.List.forEachU((. (blockId, result)) => {
                           let widgets = executeResultToWidget(result);
                           self.send(Block_AddWidgets(blockId, widgets));
                         })
                       );

                     resolve();
                   })
                |> then_(() => {
                     if (focusNextBlock) {
                       self.send(Block_FocusNextBlockOrCreate);
                     };
                     resolve();
                   })
                |> catch(error => resolve(Js.log(error)))
                |> ignore
              )
          ),
        );
      | Block_UpdateValue(blockId, newValue, diff) =>
        let blockIndex = state.blocks->getBlockIndex(blockId);
        ReasonReact.Update({
          ...state,
          stateUpdateReason: Some(action),
          blocks:
            state.blocks
            ->(
                Belt.Array.mapWithIndexU((. i, block) => {
                  let {b_data} = block;
                  if (i < blockIndex) {
                    block;
                  } else if (i == blockIndex) {
                    switch (b_data) {
                    | B_Code(bcode) => {
                        ...block,
                        b_data:
                          B_Code({
                            ...bcode,
                            bc_value: newValue,
                            bc_widgets: {
                              let removeWidgetBelowMe =
                                diff->getFirstLineFromDiff;
                              let currentWidgets = bcode.bc_widgets;
                              currentWidgets
                              ->(
                                  Belt.Array.keepU((. {Widget.lw_line, _}) =>
                                    lw_line < removeWidgetBelowMe
                                  )
                                );
                            },
                          }),
                      }
                    | B_Text(_) => {...block, b_data: B_Text(newValue)}
                    };
                  } else {
                    switch (b_data) {
                    | B_Text(_) => block
                    | B_Code(bcode) => {
                        ...block,
                        b_data: B_Code({...bcode, bc_widgets: [||]}),
                      }
                    };
                  };
                })
              )
            ->syncLineNumber,
        });
      | Block_QueueDelete(blockId) =>
        let queueTimeout = (send, b_data) => {
          let timeoutId =
            Js.Global.setTimeout(
              () => send(Block_DeleteQueued(blockId)),
              10000,
            );
          send(Block_CaptureQueuedMeta(blockId, timeoutId, b_data));
          ();
        };
        if (isLastBlock(state.blocks)) {
          switch (isEmpty(state.blocks[0].b_data)) {
          | true => ReasonReact.NoUpdate
          | _ =>
            ReasonReact.UpdateWithSideEffects(
              {
                ...state,
                blocks:
                  [|newBlock, {...state.blocks[0], b_deleted: true}|]
                  ->syncLineNumber,
                stateUpdateReason: Some(action),
                focusedBlock: None,
              },
              (({send}) => queueTimeout(send, state.blocks[0].b_data)),
            )
          };
        } else {
          let blockIndex = state.blocks->getBlockIndex(blockId);
          ReasonReact.UpdateWithSideEffects(
            {
              ...state,
              blocks:
                state.blocks
                ->(
                    Belt.Array.mapWithIndexU((. i, block) =>
                      i == blockIndex ?
                        {...state.blocks[blockIndex], b_deleted: true} : block
                    )
                  )
                ->syncLineNumber,
              stateUpdateReason: Some(action),
              focusedBlock:
                switch (state.focusedBlock) {
                | None => None
                | Some((focusedBlock, _, _)) =>
                  focusedBlock == blockId ? None : state.focusedBlock
                },
            },
            (
              ({send}) => queueTimeout(send, state.blocks[blockIndex].b_data)
            ),
          );
        };
      | Block_DeleteQueued(blockId) =>
        if (isLastBlock(state.blocks) || Belt.Array.length(state.blocks) == 0) {
          ReasonReact.Update({
            ...state,
            blocks: [|newBlock|],
            deletedBlockMeta:
              state.deletedBlockMeta
              ->(Belt.Array.keepU((. {db_id}) => db_id != blockId)),
            stateUpdateReason: Some(action),
            focusedBlock: None,
          });
        } else {
          ReasonReact.Update({
            ...state,
            blocks:
              state.blocks
              ->(Belt.Array.keepU((. {b_id}) => b_id != blockId))
              ->syncLineNumber,
            deletedBlockMeta:
              state.deletedBlockMeta
              ->(Belt.Array.keepU((. {db_id}) => db_id != blockId)),
            stateUpdateReason: Some(action),
            focusedBlock:
              switch (state.focusedBlock) {
              | None => None
              | Some((focusedBlock, _, _)) =>
                focusedBlock == blockId ? None : state.focusedBlock
              },
          });
        }
      | Block_Restore(blockId) =>
        let blockIndex = state.blocks->getBlockIndex(blockId);

        let restoreMeta =
          state.deletedBlockMeta
          ->Belt.Array.keepU(((. {db_id}) => db_id == blockId));

        let restoredBlock = {
          ...state.blocks[blockIndex],
          b_deleted: false,
          b_data: restoreMeta[0].db_data,
        };

        ReasonReact.UpdateWithSideEffects(
          {
            ...state,
            blocks:
              state.blocks
              ->(
                  Belt.Array.mapWithIndexU((. i, block) =>
                    i == blockIndex ? restoredBlock : block
                  )
                )
              ->syncLineNumber,
            deletedBlockMeta:
              state.deletedBlockMeta
              ->(Belt.Array.keepU((. {db_id}) => db_id != blockId)),
            stateUpdateReason: Some(action),
          },
          (_self => Js.Global.clearTimeout(restoreMeta[0].to_id)),
        );
      | Block_CaptureQueuedMeta(blockId, timeoutId, data) =>
        let meta = {db_id: blockId, to_id: timeoutId, db_data: data};
        ReasonReact.Update({
          ...state,
          deletedBlockMeta:
            Belt.Array.concat(state.deletedBlockMeta, [|meta|]),
          stateUpdateReason: Some(action),
        });
      | Block_Focus(blockId, blockTyp) =>
        ReasonReact.Update({
          ...state,
          stateUpdateReason: Some(action),
          focusedBlock: Some((blockId, blockTyp, FcTyp_EditorFocus)),
        })
      | Block_Blur(blockId) =>
        switch (state.focusedBlock) {
        | None => ReasonReact.NoUpdate
        | Some((focusedBlockId, _, _)) =>
          focusedBlockId == blockId ?
            ReasonReact.Update({
              ...state,
              stateUpdateReason: Some(action),
              focusedBlock: None,
            }) :
            ReasonReact.NoUpdate
        }
      | Block_Add(afterBlockId, blockTyp) =>
        let newBlockId = generateId();
        ReasonReact.Update({
          ...state,
          stateUpdateReason: Some(action),
          focusedBlock: Some((newBlockId, blockTyp, FcTyp_BlockNew)),
          blocks:
            state.blocks
            ->(
                Belt.Array.reduceU(
                  [||],
                  (. acc, block) => {
                    let {b_id} = block;
                    if (b_id != afterBlockId) {
                      Belt.Array.concat(acc, [|block|]);
                    } else {
                      Belt.Array.concat(
                        acc,
                        [|
                          block,
                          {
                            b_id: newBlockId,
                            b_data:
                              switch (blockTyp) {
                              | BTyp_Text => emptyTextBlock()
                              | BTyp_Code => emptyCodeBlock()
                              },
                            b_deleted: false,
                          },
                        |],
                      );
                    };
                  },
                )
              )
            ->syncLineNumber,
        });
      | Block_FocusUp(blockId) =>
        let upperBlock = {
          let rec loop = i =>
            if (i >= 0) {
              let {b_id} = state.blocks[i];
              if (b_id == blockId && i != 0) {
                let {b_id, b_data} = state.blocks[(i - 1)];
                switch (b_data) {
                | B_Code(_) => Some((b_id, BTyp_Code))
                | B_Text(_) => Some((b_id, BTyp_Text))
                };
              } else {
                loop(i - 1);
              };
            } else {
              None;
            };
          loop(state.blocks->Belt.Array.length - 1);
        };
        switch (upperBlock) {
        | None => ReasonReact.NoUpdate
        | Some((upperBlockId, blockTyp)) =>
          ReasonReact.Update({
            ...state,
            stateUpdateReason: Some(action),
            focusedBlock: Some((upperBlockId, blockTyp, FcTyp_BlockFocusUp)),
          })
        };
      | Block_FocusDown(blockId) =>
        let lowerBlock = {
          let length = state.blocks->Belt.Array.length;
          let rec loop = i =>
            if (i < length) {
              let {b_id} = state.blocks[i];
              if (b_id == blockId && i != length - 1) {
                let {b_id, b_data} = state.blocks[(i + 1)];
                switch (b_data) {
                | B_Code(_) => Some((b_id, BTyp_Code))
                | B_Text(_) => Some((b_id, BTyp_Text))
                };
              } else {
                loop(i + 1);
              };
            } else {
              None;
            };
          loop(0);
        };
        switch (lowerBlock) {
        | None => ReasonReact.NoUpdate
        | Some((lowerBlockId, blockTyp)) =>
          ReasonReact.Update({
            ...state,
            stateUpdateReason: Some(action),
            focusedBlock:
              Some((lowerBlockId, blockTyp, FcTyp_BlockFocusDown)),
          })
        };
      };
    },
    render: ({send, state}) =>
      <>
        state.blocks
        ->(
            Belt.Array.mapU((. {b_id, b_data, b_deleted}) =>
              b_deleted ?
                <div key=b_id id=b_id className="block__container">
                  <div className="block__deleted">
                    <h3> "This block has been removed"->str </h3>
                    <p>
                      "It will be permanently deleted after 10 seconds"->str
                    </p>
                    <div className="block__deleted--buttons">
                      <button
                        onClick=(_ => send(Block_Restore(b_id)))
                        ariaLabel="Restore block">
                        <Fi.RefreshCw />
                        "Restore"->str
                      </button>
                    </div>
                    <div className="block__deleted--progress" />
                  </div>
                  <div className="block__controls">
                    (blockControlsButtons(b_id, b_deleted, send))
                  </div>
                </div> :
                (
                  switch (b_data) {
                  | B_Code({bc_value, bc_widgets, bc_firstLineNumber}) =>
                    <div key=b_id id=b_id className="block__container">
                      <div className="source-editor">
                        <Editor_CodeBlock
                          value=bc_value
                          focused=(
                            switch (state.focusedBlock) {
                            | None => None
                            | Some((id, _blockTyp, changeTyp)) =>
                              id == b_id ? Some(changeTyp) : None
                            }
                          )
                          onChange=(
                            (newValue, diff) =>
                              send(Block_UpdateValue(b_id, newValue, diff))
                          )
                          onBlur=(() => send(Block_Blur(b_id)))
                          onFocus=(() => send(Block_Focus(b_id, BTyp_Code)))
                          onBlockUp=(() => send(Block_FocusUp(b_id)))
                          onBlockDown=(() => send(Block_FocusDown(b_id)))
                          widgets=bc_widgets
                          readOnly
                          firstLineNumber=bc_firstLineNumber
                          lang
                        />
                      </div>
                      <div className="block__controls">
                        (
                          readOnly ?
                            React.null :
                            blockControlsButtons(b_id, b_deleted, send)
                        )
                      </div>
                    </div>
                  | B_Text(text) =>
                    <div key=b_id id=b_id className="block__container">
                      <div className="text-editor">
                        <Editor_TextBlock
                          value=text
                          focused=(
                            switch (state.focusedBlock) {
                            | None => None
                            | Some((id, _blockTyp, changeTyp)) =>
                              id == b_id ? Some(changeTyp) : None
                            }
                          )
                          onBlur=(() => send(Block_Blur(b_id)))
                          onFocus=(() => send(Block_Focus(b_id, BTyp_Text)))
                          onBlockUp=(() => send(Block_FocusUp(b_id)))
                          onBlockDown=(() => send(Block_FocusDown(b_id)))
                          onChange=(
                            (newValue, diff) =>
                              send(Block_UpdateValue(b_id, newValue, diff))
                          )
                          readOnly
                        />
                      </div>
                      (
                        readOnly ?
                          React.null :
                          <div className="block__controls">
                            (blockControlsButtons(b_id, b_deleted, send))
                          </div>
                      )
                    </div>
                  }
                )
            )
          )
        ->ReasonReact.array
      </>,
  };
};
