/// <reference types="Cypress" />

const faker = require("faker");

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
  aliasSaveButton,
} from "../../helpers/editor_helpers";

context("Block controls > Add", () => {
  it("should add new code block and focus on it", () => {
    cy.visit("new/reason");
    assertBlocks(1);
    assertCodeBlocks(1);
    assertTextBlocks(0);

    cy.get(".block__container")
      .first()
      .find(`button[aria-label="Add code block"]`)
      .click();

    assertCodeBlocks(2);
    assertTextBlocks(0);

    cy.get(".block__container")
      .eq(1)
      .find("textarea")
      .focused();

    cy.get(".block__container")
      .eq(1)
      .find(`button[aria-label="Add code block"]`)
      .click();

    assertCodeBlocks(3);
    assertTextBlocks(0);

    cy.get(".block__container")
      .eq(2)
      .find("textarea")
      .focused();
  });
  it("should add new text block and focus on it", () => {
    cy.visit("new/reason");
    assertBlocks(1);
    assertCodeBlocks(1);

    cy.get(".block__container")
      .first()
      .find(`button[aria-label="Add text block"]`)
      .click();

    assertCodeBlocks(1);
    assertTextBlocks(1);

    cy.get(".block__container")
      .eq(1)
      .find("textarea")
      .focused();
    cy.get(".block__container")
      .eq(1)
      .find(`button[aria-label="Add code block"]`)
      .click();

    assertCodeBlocks(2);
    assertTextBlocks(1);
    cy.get(".block__container")
      .eq(2)
      .find("textarea")
      .focused();
  });
});

context("Block controls > delete and restore", () => {
  it("delete block -> timeout 10 seconds -> delete permantely", () => {
    cy.visit("new/reason");
    shortcut("{shift}{enter}");
    shortcut("{shift}{enter}");
    shortcut("{shift}{enter}");
    shortcut("{shift}{enter}");
    shortcut("{shift}{enter}");

    assertCodeBlocks(6);

    cy.get(".block__container")
      .eq(1)
      .find(`button[aria-label="Delete block"]`)
      .click();

    assertCodeBlocks(5);
    cy.get(".block__container")
      .eq(1)
      .find(".block__deleted", {
        timeout: 10000,
      })
      .should("not.exist");
  });
  it("have restore button in toolbar as well", () => {
    cy.visit("new/reason");
    shortcut("{shift}{enter}");
    shortcut("{shift}{enter}");

    assertCodeBlocks(3);

    cy.get(".block__container")
      .eq(1)
      .find(".block__controls")
      .find(`button[aria-label="Delete block"]`)
      .click();
    assertCodeBlocks(2);

    cy.get(".block__container")
      .eq(1)
      .find(".block__controls")
      .find(`button[aria-label="Delete block"]`)
      .should("not.exist");

    cy.get(".block__container")
      .eq(1)
      .find(".block__controls")
      .find(`button[aria-label="Restore block"]`)
      .click();
    assertCodeBlocks(3);
  });
  it("can restore temporaty deleted block", () => {
    cy.visit("new/reason");
    shortcut("{shift}{enter}");
    shortcut("{shift}{enter}");
    shortcut("{shift}{enter}");
    shortcut("{shift}{enter}");
    shortcut("{shift}{enter}");

    assertCodeBlocks(6);

    cy.get(".block__container")
      .eq(1)
      .find(`button[aria-label="Delete block"]`)
      .click();
    assertCodeBlocks(5);

    cy.get(".block__container")
      .eq(1)
      .find(".block__deleted")
      .find(`button[aria-label="Restore block"]`)
      .click();
    assertCodeBlocks(6);
  });
  it("can delete immediately", () => {
    cy.visit("new/reason");
    shortcut("{shift}{enter}");
    shortcut("{shift}{enter}");
    shortcut("{shift}{enter}");
    shortcut("{shift}{enter}");
    shortcut("{shift}{enter}");

    assertCodeBlocks(6);

    cy.get(".block__container")
      .eq(1)
      .find(`button[aria-label="Delete block"]`)
      .click();
    assertCodeBlocks(5);

    cy.get(".block__container")
      .eq(1)
      .find(".block__deleted")
      .find(`button[aria-label="Delete block immediately"]`)
      .click();
    assertBlocks(5);
  });
  it("delete immediately should not remove last block - #182", () => {
    cy.visit("new/reason");
    shortcut("{shift}{enter}");

    assertCodeBlocks(2);
    typeBlock(0, "0");
    typeBlock(1, "1");
    cy.get(".block__container")
      .eq(0)
      .find(`button[aria-label="Delete block"]`)
      .click();
    assertCodeBlocks(1);

    cy.wait(2000);
    cy.get(".block__container")
      .eq(0)
      .find(".block__deleted")
      .find(`button[aria-label="Delete block immediately"]`)
      .click();
    assertBlocks(1);

    cy.wait(10000);
    assertBlocks(1);
    assertLastBlockValue("1");
  });
  it("remove temporary deleted block from execution", () => {
    cy.visit("new/reason");
    cy.get(".block__container")
      .eq(0)
      .as("block1")
      .find("textarea")
      .as("code1")
      .type("let a = 1;", {
        force: true,
      });
    shortcut("{shift}{enter}");
    assertCodeBlocks(2);
    cy.get(".block__container")
      .eq(1)
      .as("block2")
      .find("textarea")
      .as("code2")
      .type("print_int(a);", {
        force: true,
      });
    shortcut("{ctrl}{enter}");
    assertValue(2);

    cy.get("@block1")
      .find(`button[aria-label="Delete block"]`)
      .click();
    assertCodeBlocks(1);
    shortcut("{ctrl}{enter}");

    assertValue(0);
    assertErrorsOrWarnings(1);
  });

  it("sync line number when temporary block shows up", () => {
    cy.visit("new/reason");
    shortcut("{shift}{enter}");
    shortcut("{shift}{enter}");
    assertCodeBlocks(3);

    cy.get(".block__container")
      .eq(1)
      .find(`button[aria-label="Delete block"]`)
      .click();
    assertCodeBlocks(2);

    cy.window().then(win => {
      expect(win.editor.getOption("firstLineNumber")).to.equal(2);
    });
  });

  it("delete queued should trigger Editor_Note state update", () => {
    cy.visit("new/reason");
    aliasSaveButton();

    typeBlock(0, "let a = 1;");
    shortcut("{ctrl}s");
    cy.get("@save").should("be.disabled");
    cy.get(".block__container")
      .eq(0)
      .find(`button[aria-label="Delete block"]`)
      .click();
    cy.get("@save").should("be.enabled");
  });
});
