[%%debugger.chrome];
Modules.require("./Editor_Blocks.css");

open Utils;
open Editor_Types;
open Editor_Types.Block;
open Editor_Blocks_Utils;

type action =
  | Block_Add(id, blockTyp)
  | Block_Execute(bool, blockTyp)
  | Block_FocusNextBlockOrCreate(blockTyp)
  | Block_QueueDelete(id)
  | Block_DeleteQueued(id)
  | Block_CaptureQueuedMeta(id, Js.Global.timeoutId)
  | Block_Restore(id)
  | Block_Focus(id, blockTyp)
  | Block_Blur(id)
  | Block_UpdateValue(id, string, CodeMirror.EditorChange.t)
  | Block_AddWidgets(id, array(Widget.t))
  | Block_FocusUp(id)
  | Block_FocusDown(id)
  | Block_ChangeLanguage
  | Block_PrettyPrint
  | Block_CleanBlocksCopy
  | Block_MapRefmtToBlocks(list((id, string)));

type state = {
  lang,
  blocks: array(block),
  blocksCopy: option(array(block)),
  deletedBlockMeta: array((id, Js.Global.timeoutId)),
  stateUpdateReason: option(action),
  focusedBlock: option((id, blockTyp, focusChangeType)),
};

module Actions = {
  let cleanBlocksCopy = (action, state) =>
    ReasonReact.Update({
      ...state,
      blocksCopy: None,
      stateUpdateReason: Some(action),
    });
  let prettyPrint = (_action, state) =>
    switch (state.lang) {
    | ML =>
      Notify.info("Prettify ML code is not currently supported");
      ReasonReact.NoUpdate;
    | RE =>
      ReasonReact.SideEffects(
        (
          self => {
            let id =
              Toplevel_Consumer.refmt(
                PrettyPrintRe,
                self.state.blocks->codeBlockDataPairs,
                fun
                | Belt.Result.Error(error) => Notify.error(error)
                | Belt.Result.Ok({hasError, result}) =>
                  if (hasError) {
                    /* TODO: Map syntax error to block position */
                    Notify.error(
                      "An error happens while formatting your code. It's usually a syntax error",
                    );
                  } else {
                    let result =
                      result
                      ->Belt.List.reduce(
                          [],
                          (
                            (acc, {refmt_id, refmt_value}) =>
                              switch (refmt_value) {
                              | Ok(code) => [(refmt_id, code), ...acc]
                              | Error(_) => acc
                              }
                          ),
                        );
                    self.send(Block_MapRefmtToBlocks(result));
                  },
              );
            self.onUnmount(() => Toplevel_Consumer.cancel(id));
          }
        ),
      )
    };
  /*
   * Focus up helper
   * - Find current block index
   * - If it's the FIRST or not in blocks (should never happen) then ignore
   * - Else focus on upper block
   */
  let focusUp = (action, state, blockId) => {
    let blockIndex =
      state.blocks->arrayFindIndex(({b_id}) => blockId == b_id);
    switch (blockIndex) {
    | None
    | Some(0) => ReasonReact.NoUpdate
    | Some(index) =>
      let upperBlockIndex = index - 1;
      let {b_data, b_id} =
        state.blocks->Belt.Array.getUnsafe(upperBlockIndex);
      let blockTyp = blockDataToBlockTyp(b_data);
      ReasonReact.Update({
        ...state,
        stateUpdateReason: Some(action),
        focusedBlock: Some((b_id, blockTyp, FcTyp_BlockFocusUp)),
      });
    };
  };
  /*
   * Focus down helper
   * - Find current block index
   * - If it's the LAST or not in blocks (should never happen) then ignore
   * - Else focus on lower block
   */
  let focusDown = (action, state, blockId) => {
    let length = state.blocks->Belt.Array.length;
    let blockIndex =
      state.blocks->arrayFindIndex(({b_id}) => blockId == b_id);
    switch (blockIndex) {
    | None => ReasonReact.NoUpdate
    | Some(blockIndex) when blockIndex == length - 1 => ReasonReact.NoUpdate
    | Some(index) =>
      let lowerBlockIndex = index + 1;
      let {b_data, b_id} =
        state.blocks->Belt.Array.getUnsafe(lowerBlockIndex);
      let blockTyp = blockDataToBlockTyp(b_data);
      ReasonReact.Update({
        ...state,
        stateUpdateReason: Some(action),
        focusedBlock: Some((b_id, blockTyp, FcTyp_BlockFocusDown)),
      });
    };
  };
  /*
   * Add a new code/text block afterBlockId
   * It also focus on the newly added block
   */
  let add = (action, state, afterBlockId, blockTyp) => {
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
  };
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
      ~onExecute,
      ~registerExecuteCallback=?,
      ~registerShortcut: option(Shortcut.subscribeFun)=?,
      _children: React.childless,
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
        {...state, lang};
      } else {
        state;
      },
    didMount: self => {
      self.send(Block_Execute(false, BTyp_Code));
      switch (registerExecuteCallback) {
      | None => ()
      | Some(register) =>
        register(() => self.send(Block_Execute(false, BTyp_Code)))
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
              self.send(Block_Execute(false, BTyp_Code));
            },
          );
        let unReg2 =
          registerShortcut(
            ~global=true,
            "shift+enter",
            event => {
              open Webapi.Dom;

              event->KeyboardEvent.preventDefault;
              self.send(Block_Execute(true, BTyp_Code));
            },
          );
        let unReg3 =
          registerShortcut(
            ~global=true,
            "mod+shift+enter",
            event => {
              open Webapi.Dom;

              event->KeyboardEvent.preventDefault;
              self.send(Block_Execute(true, BTyp_Text));
            },
          );
        let unReg4 =
          registerShortcut(
            ~global=true,
            "mod+shift+i",
            event => {
              open Webapi.Dom;
              event->KeyboardEvent.preventDefault;
              self.send(Block_PrettyPrint);
            },
          );
        self.onUnmount(() => {
          unReg();
          unReg2();
          unReg3();
          unReg4();
        });
      };
    },
    didUpdate: ({oldSelf, newSelf}) => {
      if (oldSelf.state.lang != lang) {
        newSelf.send(Block_ChangeLanguage);
      };
      if (oldSelf.state.blocks !== newSelf.state.blocks) {
        let cleanBlocksCopy = () =>
          switch (newSelf.state.blocksCopy) {
          | None => ()
          | Some(_) => newSelf.send(Block_CleanBlocksCopy)
          };
        switch (newSelf.state.stateUpdateReason) {
        | None => ()
        | Some(action) =>
          switch (action) {
          | Block_Focus(_, _)
          | Block_Blur(_)
          | Block_AddWidgets(_, _)
          | Block_FocusUp(_)
          | Block_FocusDown(_)
          | Block_CaptureQueuedMeta(_)
          | Block_QueueDelete(_)
          | Block_ChangeLanguage
          | Block_CleanBlocksCopy
          | Block_FocusNextBlockOrCreate(_)
          | Block_Execute(_, _) => ()
          | Block_PrettyPrint
          | Block_MapRefmtToBlocks(_)
          | Block_Add(_, _)
          | Block_DeleteQueued(_)
          | Block_Restore(_)
          | Block_UpdateValue(_, _, _) => onUpdate(newSelf.state.blocks)
          };

          switch (action) {
          | Block_Add(_, _)
          | Block_Restore(_)
          | Block_DeleteQueued(_)
          | Block_PrettyPrint => cleanBlocksCopy()
          | Block_UpdateValue(_, _, diff) =>
            switch (diff->CodeMirror.EditorChange.originGet) {
            | "setValue" => ()
            | _ => cleanBlocksCopy()
            }
          | _ => ()
          };
        };
      };
    },
    reducer: (action, state) =>
      switch (action) {
      | Block_CleanBlocksCopy => Actions.cleanBlocksCopy(action, state)
      | Block_PrettyPrint => Actions.prettyPrint(action, state)
      | Block_ChangeLanguage =>
        switch (state.blocksCopy) {
        | None =>
          ReasonReact.UpdateWithSideEffects(
            {...state, blocksCopy: Some(state.blocks)},
            (
              self => {
                let operation =
                  switch (state.lang) {
                  | ML => Toplevel.Types.ReToMl
                  | RE => Toplevel.Types.MlToRe
                  };
                let id =
                  Toplevel_Consumer.refmt(
                    operation,
                    self.state.blocks->codeBlockDataPairs,
                    fun
                    | Belt.Result.Error(error) => Notify.error(error)
                    | Belt.Result.Ok({hasError, result}) =>
                      if (hasError) {
                        /* TODO: Map syntax error to block position */
                        Notify.error(
                          "An error happens while formatting your code. It might be a syntax error",
                        );
                      } else {
                        let result =
                          result
                          ->Belt.List.reduce(
                              [],
                              (
                                (acc, {refmt_id, refmt_value}) =>
                                  switch (refmt_value) {
                                  | Ok(code) => [(refmt_id, code), ...acc]
                                  | Error(_) => acc
                                  }
                              ),
                            );
                        self.send(Block_MapRefmtToBlocks(result));
                      },
                  );
                self.onUnmount(() => Toplevel_Consumer.cancel(id));
              }
            ),
          )
        | Some(blocksCopy) =>
          ReasonReact.UpdateWithSideEffects(
            {...state, blocksCopy: None, blocks: blocksCopy},
            (self => self.send(Block_Execute(false, BTyp_Code))),
          )
        }

      | Block_MapRefmtToBlocks(results) =>
        ReasonReact.UpdateWithSideEffects(
          {
            ...state,
            stateUpdateReason: Some(action),
            blocks:
              state.blocks
              ->(
                  Belt.Array.mapU((. block) => {
                    let {b_data, b_id, b_deleted} = block;
                    b_deleted ?
                      block :
                      (
                        switch (b_data) {
                        | B_Code(bcode) =>
                          let mappedBlock =
                            results
                            ->Belt.List.getBy(
                                (((id, _code)) => id == b_id),
                              );
                          switch (mappedBlock) {
                          | None => block
                          | Some((_id, code)) => {
                              ...block,
                              b_data:
                                B_Code({
                                  ...bcode,
                                  /* TODO: Don't remove widgets but executing and replace with latest widget */
                                  bc_widgets: [||],
                                  bc_value: code,
                                }),
                            }
                          };
                        | B_Text(_) => block
                        }
                      );
                  })
                ),
          },
          (({send}) => send(Block_Execute(false, BTyp_Code))),
        )
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
      | Block_FocusNextBlockOrCreate(blockTyp) =>
        let blockLength = state.blocks->Belt.Array.length;

        let currentBlockIndex =
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
        if (currentBlockIndex == blockLength - 1) {
          ReasonReact.SideEffects(
            (
              ({send}) =>
                send(
                  Block_Add(findBlockId(currentBlockIndex)->fst, blockTyp),
                )
            ),
          );
        } else if (currentBlockIndex < blockLength - 1) {
          let (nextBlockId, nextBlockTyp) =
            findBlockId(currentBlockIndex + 1);
          ReasonReact.Update({
            ...state,
            stateUpdateReason: Some(action),
            focusedBlock:
              Some((
                nextBlockId,
                nextBlockTyp,
                FcTyp_BlockExecuteAndFocusNextBlock,
              )),
          });
        } else {
          ReasonReact.NoUpdate;
        };
      | Block_Execute(focusNextBlock, blockTyp) =>
        let allCodeToExecute = codeBlockDataPairs(state.blocks);

        ReasonReact.SideEffects(
          (
            self => {
              if (focusNextBlock) {
                self.send(Block_FocusNextBlockOrCreate(blockTyp));
              };
              onExecute(true);
              let id =
                Toplevel_Consumer.execute(
                  lang,
                  allCodeToExecute,
                  fun
                  | Belt.Result.Error(error) => {
                      onExecute(false);
                      Notify.error(error);
                    }
                  | Belt.Result.Ok(blocks) => {
                      onExecute(false);
                      blocks
                      ->(
                          Belt.List.forEachU(
                            (. {Toplevel.Types.id: blockId, result}) => {
                            let widgets = executeResultToWidget(result);
                            self.send(Block_AddWidgets(blockId, widgets));
                          })
                        );
                    },
                );

              self.onUnmount(() => Toplevel_Consumer.cancel(id));
            }
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
        let queueTimeout = self => {
          let timeoutId =
            Js.Global.setTimeout(
              () => self.ReasonReact.send(Block_DeleteQueued(blockId)),
              10000,
            );
          self.onUnmount(() => Js.Global.clearTimeout(timeoutId));
          self.ReasonReact.send(Block_CaptureQueuedMeta(blockId, timeoutId));
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
              (self => queueTimeout(self)),
            )
          };
        } else {
          ReasonReact.UpdateWithSideEffects(
            {
              ...state,
              blocks:
                state.blocks
                ->(
                    Belt.Array.mapU((. block) => {
                      let {b_id} = block;
                      b_id == blockId ? {...block, b_deleted: true} : block;
                    })
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
            (self => queueTimeout(self)),
          );
        };
      | Block_DeleteQueued(blockId) =>
        if (isLastBlock(state.blocks) || Belt.Array.length(state.blocks) == 0) {
          ReasonReact.Update({
            ...state,
            blocks: [|newBlock|],
            deletedBlockMeta:
              state.deletedBlockMeta
              ->(
                  Belt.Array.keepU((. (timeoutBlockId, _timeoutId)) =>
                    timeoutBlockId != blockId
                  )
                ),
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
              ->(
                  Belt.Array.keepU((. (timeoutBlockId, _timeoutId)) =>
                    timeoutBlockId != blockId
                  )
                ),
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
        let timeoutId =
          state.deletedBlockMeta
          ->arrayFind(
              (((timeBlockId, _timeoutId)) => timeBlockId == blockId),
            );
        switch (timeoutId) {
        | None => ReasonReact.NoUpdate
        | Some((_timeoutBlockId, timeoutId)) =>
          ReasonReact.UpdateWithSideEffects(
            {
              ...state,
              blocks:
                state.blocks
                ->(
                    Belt.Array.mapU((. block) => {
                      let {b_id} = block;

                      b_id == blockId ? {...block, b_deleted: false} : block;
                    })
                  )
                ->syncLineNumber,
              deletedBlockMeta:
                state.deletedBlockMeta
                ->(
                    Belt.Array.keepU((. (timeBlockId, _timeoutId)) =>
                      timeBlockId != blockId
                    )
                  ),
              stateUpdateReason: Some(action),
            },
            (_self => Js.Global.clearTimeout(timeoutId)),
          )
        };
      | Block_CaptureQueuedMeta(blockId, timeoutId) =>
        ReasonReact.Update({
          ...state,
          deletedBlockMeta:
            Belt.Array.concat(
              state.deletedBlockMeta,
              [|(blockId, timeoutId)|],
            ),
          stateUpdateReason: Some(action),
        })
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
        Actions.add(action, state, afterBlockId, blockTyp)
      | Block_FocusUp(blockId) => Actions.focusUp(action, state, blockId)
      | Block_FocusDown(blockId) => Actions.focusDown(action, state, blockId)
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
