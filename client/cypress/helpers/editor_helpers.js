const assertBlocks = howManyBlocks =>
  cy.get(".block__container").should("have.length", howManyBlocks);
const assertCodeBlocks = howManyBlocks =>
  cy
    .get(".block__container > .source-editor")
    .should("have.length", howManyBlocks);
const assertTextBlocks = howManyBlocks =>
  cy
    .get(".block__container > .text-editor")
    .should("have.length", howManyBlocks);

const assertErrorsOrWarnings = howMany =>
  cy.get(".widget__lint").should("have.length", howMany);
const assertValue = howMany =>
  cy.get(".widget__value").should("have.length", howMany);
const shortcut = shortcut =>
  cy
    .get("body")
    .focus()
    .type(shortcut);

const assertLastBlockValue = value => {
  cy.window().then(win => {
    expect(win.editor.getValue()).to.equal(value);
  });
};

export {
  assertBlocks,
  assertCodeBlocks,
  assertTextBlocks,
  assertErrorsOrWarnings,
  assertValue,
  shortcut,
  assertLastBlockValue,
};
