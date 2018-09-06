/// <reference types="Cypress" />

const faker = require("faker");

import {
  assertBlocks,
  assertCodeBlocks,
  assertTextBlocks,
  assertErrorsOrWarnings,
  assertValue,
  shortcut,
  aliasSaveButton,
} from "../../helpers/editor_helpers";

context("keyboard shortcuts", () => {
  beforeEach(() => {
    cy.visit("new/reason");
  });

  it("ctrl+s for save", () => {
    let title = faker.lorem.words();

    aliasSaveButton();

    cy.get("@save").should("be.disabled");

    cy.get(".EditorNote__metadata")
      .find("input")
      .first()
      .as("title")
      .type(title);

    cy.get("@save").should("be.enabled");

    cy.get("textarea")
      .first()
      .type("let a: string = 1;", {
        force: true,
      });
    shortcut("{ctrl}s");

    cy.get("@save").should("be.disabled");
    // cy.get("@save").should("have.text", "Saving");
    cy.get("@save").should("have.text", "Save");

    // Repeat
    cy.get("@title").type(faker.lorem.words());
    cy.get("@save").should("be.enabled");

    shortcut("{ctrl}s");

    cy.get("@save").should("be.disabled");
  });

  it("shift+enter execute and focus next block/or create new  code block", () => {
    assertBlocks(1);
    cy.get(".block__container")
      .first()
      .find("textarea")
      .as("block1")
      .type("let a = 1;", {
        force: true,
      });

    shortcut("{shift}{enter}");
    assertValue(1);
    assertBlocks(2);

    cy.get(".block__container")
      .eq(1)
      .find("textarea")
      .as("block2")
      .type("print_int(a);", {
        force: true,
      });

    shortcut("{shift}{enter}");
    assertValue(2);
    assertBlocks(3);

    shortcut("{shift}{enter}");
    shortcut("{shift}{enter}");
    assertBlocks(5);

    cy.get(".block__container")
      .eq(4)
      .find("textarea")
      .focused();

    cy.get(".block__container")
      .eq(2)
      .find("textarea")
      .focus();

    shortcut("{shift}{enter}");
    cy.get(".block__container")
      .eq(3)
      .find("textarea")
      .focused();
    assertBlocks(5);

    shortcut("{shift}{enter}");
    cy.get(".block__container")
      .eq(4)
      .find("textarea")
      .focused();
    assertBlocks(5);
  });

  it("shift+enter+ctrl execute and focus next block/or create new text block", () => {
    assertBlocks(1);
    cy.get(".block__container")
      .first()
      .find("textarea")
      .as("block1")
      .type("let a = 1;", {
        force: true,
      });

    shortcut("{shift}{ctrl}{enter}");
    assertValue(1);
    assertBlocks(2);
    assertCodeBlocks(1);
    assertTextBlocks(1);

    shortcut("{shift}{ctrl}{enter}");
    assertCodeBlocks(1);
    assertTextBlocks(2);

    cy.get(".block__container .text-editor")
      .first()
      .find("textarea")
      .type("this thing is working", {
        force: true,
      });

    shortcut("{shift}{ctrl}{enter}");
    assertCodeBlocks(1);
    assertTextBlocks(2);

    shortcut("{shift}{ctrl}{enter}");
    shortcut("{shift}{ctrl}{enter}");
    assertCodeBlocks(1);
    assertTextBlocks(4);

    shortcut("{shift}{enter}");
    cy.get(".block__container .text-editor")
      .eq(3)
      .find("textarea")
      .focus();

    shortcut("{shift}{ctrl}{enter}");
    assertCodeBlocks(2);
    assertTextBlocks(4);
  });

  it("ctrl+enter should execute without creating new block", () => {
    assertBlocks(1);

    cy.get(".block__container")
      .first()
      .find("textarea")
      .as("block1")
      .type("let a: string = 1;", {
        force: true,
      });
    assertErrorsOrWarnings(0);

    shortcut("{ctrl}{enter}");
    assertErrorsOrWarnings(1);

    cy.get("@block1")
      .clear({
        force: true,
      })
      .type("let a = 1;", {
        force: true,
      });
    assertErrorsOrWarnings(0);

    shortcut("{ctrl}{enter}");

    assertErrorsOrWarnings(0);
    cy.get("@block1")
      .get(".widget__value")
      .should("contain", "let a: int = 1;");
  });
});

context("Edge cases", () => {
  it("#105: should not prevent editing title after focusing on a block", () => {
    // https://github.com/Sketch-sh/sketch-sh/issues/105

    cy.visit("new/reason");

    cy.get(".EditorNote__metadata")
      .find("input")
      .first()
      .as("title")
      .type(faker.lorem.words());

    cy.get(".block__container")
      .first()
      .find("textarea")
      .as("block1")
      .type("let a = 1;", {
        force: true,
      });

    cy.get("@title")
      .focus()
      .clear()
      .type("new title");

    cy.get("@title")
      .should("have.value", "new title")
      .focused();
  });

  it("should display value in correct order", () => {
    cy.visit("new/reason");
    cy.get(".block__container")
      .first()
      .find("textarea")
      .as("block1")
      .type("let x = 1; let y = 2; let z = 3;", {
        force: true,
      });
    shortcut("{ctrl}{enter}");
    assertValue(3);

    cy.get(".widget__value").as("value");
    cy.get("@value")
      .eq(0)
      .should("have.text", "let x: int = 1;\n");
    cy.get("@value")
      .eq(1)
      .should("have.text", "let y: int = 2;\n");
    cy.get("@value")
      .eq(2)
      .should("have.text", "let z: int = 3;\n");
  });

  it("#112: ctrl+z should not clear content after mount", () => {
    let content = "let a = 1;";
    cy.visit("new/reason");
    cy.get(".block__container")
      .first()
      .find("textarea")
      .as("block1")
      .type(content, {
        force: true,
      });
    shortcut("{ctrl}s");

    cy.url().should("match", /s\/.+/, "should be new route");
    cy.reload(true);

    /* Wait for editor to mount */
    assertValue(1);

    cy.window().then(win => {
      expect(win.editor.getValue()).to.equal(content);
    });
    cy.get("@block1").type("{ctrl}z", {
      force: true,
    });
    cy.window().then(win => {
      expect(win.editor.getValue()).to.equal(content);
    });
  });
  it("should not report lineWidget at line -1", () => {
    let content = `module AdgroupStore = {
  let get = adgroupID => "THIS IS AN ADGROUP";
};

module PageStore = {
  let get = pageID => "THIS IS A PAGE";
};


let containerize = (~pageID, ~adgroupID, component) => {
  component(~page=PageStore.get(pageID), ~adgroup=AdgroupStore.get(adgroupID), ~children=[], ());
};

module MyComponent = {
  let createElement = (~page, ~adgroup, ~children, ()) => {
    ();
    ();
  };
  let createElement = containerize(createElement);
};

print_string("SDFlj");

let renderThis = <MyComponent adgroupID="34" pageID="99"/>`;
    cy.visit("new/reason");
    assertBlocks(1);

    cy.window().then(win => {
      expect(win.editor.setValue(content));
      shortcut("{ctrl}{enter}");
      assertErrorsOrWarnings(1);
    });
  });
});
