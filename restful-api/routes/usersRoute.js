const express = require('express');
const userController = require('../controllers/userController');

const router = express.Router();

router.post('/create', async (req, res, next) => {

    console.log('User create: ', req.body);

    const name = req.body.name;
    const email = req.body.email;
    const password = req.body.password;
    let dbUser = null;

    try {
        dbUser = await userController.createUser(name, email, password);
    } catch (ex) {
        console.log('Error creating user', ex);
        dbUser = null;
    }

    dbUser===null?res.sendStatus(400):res.sendStatus(201);
});

router.put('/addRolesForProject', async (req, res, next) => {

    console.log('User addRolesForProject: ', req.body);

    const project = req.body.project;
    const email = req.body.email;
    const roles = req.body.roles;
    let error = false;

    try {
        await userController.addRolesForProject(project, email, roles);
    } catch (ex) {
        console.log('Error adding roles for project', ex);
        error = true;
    }

    error===null?res.sendStatus(400):res.sendStatus(204);
});

router.put('/removeRolesForProject', async (req, res, next) => {

    console.log('User removeRolesForProject: ', req.body);

    const project = req.body.project;
    const email = req.body.email;
    const roles = req.body.roles;
    let error = false;

    try {
        await userController.removeRolesForProject(project, email, roles);
    } catch (ex) {
        console.log('Error removing roles for project', ex);
        error = true;
    }

    error===null?res.sendStatus(400):res.sendStatus(204);
});

module.exports = router;