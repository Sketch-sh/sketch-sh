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
const assertStdout = howMany =>
  cy.get(".widget__stdout").should("have.length", howMany);

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

const typeTitle = title => {
  cy.get(".EditorNote__metadata")
    .find("input")
    .first()
    .as("title")
    .type(title);
};
const typeBlock = (eq = 0, content) => {
  cy.get(".block__container")
    .eq(eq)
    .find("textarea")
    .type(content, { force: true });
};
export {
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
};
