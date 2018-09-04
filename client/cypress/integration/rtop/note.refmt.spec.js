/// <reference types="Cypress" />

import {
  assertBlocks,
  assertCodeBlocks,
  assertTextBlocks,
  assertStdout,
  assertErrorsOrWarnings,
  assertValue,
  shortcut,
  assertLastBlockValue,
  typeTitle,
  typeBlock,
} from "../../helpers/editor_helpers";

context("refmt", () => {
  it("refmt with shortcut", () => {
    cy.visit("new/reason");
    typeBlock(
      0,
      `switch (true) { 
    | true => () 
    | false => () 
    }`
    );
    shortcut("{ctrl}{shift}i");
    assertLastBlockValue("true ? () : ();");
  });
  it("execute after refmt", () => {
    assertValue(1);
  });
});

context("refmt - bugs", () => {
  it("should not restore deleted blocks", () => {
    cy.visit("new/reason");
    typeBlock(0, `let a = 1;`);
    cy.get(".block__container")
      .first()
      .find(`button[aria-label="Add code block"]`)
      .click();
    typeBlock(1, `let a = 2;`);
    cy.get(".block__container")
      .eq(1)
      .find(`button[aria-label="Add code block"]`)
      .click();
    typeBlock(2, `print_int(a);`);
    shortcut("{ctrl}{enter}");
    cy.get(".widget__stdout").contains("2");

    cy.get(".block__container")
      .eq(1)
      .find(`button[aria-label="Delete block"]`)
      .click();
    shortcut("{ctrl}{shift}i");

    cy.get(".block__deleted");
    shortcut("{ctrl}{enter}");
    cy.get(".widget__stdout").contains("1");
  });
});
